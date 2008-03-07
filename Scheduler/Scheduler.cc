// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

/** \file
    \brief Scheduler non-inline non-template implementation

    \idea Multithreading support: To support multithreading, the
    static member Scheduler::instance() must return a thread-local
    value (that is Scheduler::instance() must allocate one Scheduler
    instance per thread). Another possibility would be to distribute
    the async load unto several threads (one scheduler for multiple
    threads)
 */

// Here a basic concept of how to add signal support to the scheduler:
//
// ... no, I had overlooked one race condition. So back to the signal-pipe approach ...

#include "Scheduler.hh"
//#include "Scheduler.ih"

// Custom includes
#include "../Utils/senfassert.hh"
#include <errno.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include "../Utils/Exception.hh"

static const int EPollInitialSize = 16;

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::Scheduler::Scheduler()
    : files_(0), timerIdCounter_(0), epollFd_ (epoll_create(EPollInitialSize)), terminate_(false),
      eventTime_(0), eventEarly_(ClockService::milliseconds(11)), eventAdjust_(0)
{
    if (epollFd_<0)
        SENF_THROW_SYSTEM_EXCEPTION("");

    if (::pipe(sigpipe_) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");

    int flags (::fcntl(sigpipe_[1],F_GETFL));
    if (flags < 0) 
        SENF_THROW_SYSTEM_EXCEPTION("");
    flags |= O_NONBLOCK;
    if (::fcntl(sigpipe_[1], F_SETFL, flags) < 0) 
        SENF_THROW_SYSTEM_EXCEPTION("");

    ::epoll_event ev;
    ::memset(&ev, 0, sizeof(ev));
    ev.events = EV_READ;
    ev.data.fd = sigpipe_[0];
    if (::epoll_ctl(epollFd_, EPOLL_CTL_ADD, sigpipe_[0], &ev) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
}

prefix_ void senf::Scheduler::registerSignal(unsigned signal, SimpleCallback const & cb)
{
    ::sigset_t sig;
    ::sigemptyset(&sig);
    if (::sigaddset(&sig, signal) < 0)
        throw InvalidSignalNumberException();
    ::sigprocmask(SIG_BLOCK, &sig, 0);
    ::sigaddset(&sigset_, signal);
    if (sigHandlers_.size() <= signal)
        sigHandlers_.resize(signal+1);
    sigHandlers_[signal] = cb;

    registerSigHandlers();
}

prefix_ void senf::Scheduler::unregisterSignal(unsigned signal)
{
    if (::sigdelset(&sigset_, signal) < 0)
        throw InvalidSignalNumberException();
    sigHandlers_[signal] = 0;
    ::signal(signal, SIG_DFL);
    registerSigHandlers();
}

prefix_ void senf::Scheduler::do_add(int fd, FdCallback const & cb, int eventMask)
{
    if (eventMask == 0)
        return;

    FdTable::iterator i (fdTable_.find(fd));
    int action (EPOLL_CTL_MOD);
    if (i == fdTable_.end()) {
        action = EPOLL_CTL_ADD;
        i = fdTable_.insert(std::make_pair(fd, EventSpec())).first;
    }
    if (i->second.epollMask() == 0) {
        action = EPOLL_CTL_ADD;
        fdErase_.erase( std::remove(fdErase_.begin(), fdErase_.end(), unsigned(fd)),
                        fdErase_.end() );
    }

    if (eventMask & EV_READ)  i->second.cb_read = cb;
    if (eventMask & EV_PRIO)  i->second.cb_prio = cb;
    if (eventMask & EV_WRITE) i->second.cb_write = cb;

    epoll_event ev;
    memset(&ev,0,sizeof(ev));
    ev.events = i->second.epollMask();
    ev.data.fd = fd;

    if (! i->second.file && epoll_ctl(epollFd_, action, fd, &ev) < 0) {
        if (errno == EPERM) {
            // Argh ... epoll does not support ordinary files :-( :-(
            i->second.file = true;
            ++ files_;
        }
        else
            SENF_THROW_SYSTEM_EXCEPTION("::epoll_ctl()");
    }
}

prefix_ void senf::Scheduler::do_remove(int fd, int eventMask)
{
    if (eventMask == 0)
        return;

    FdTable::iterator i (fdTable_.find(fd));
    if (i == fdTable_.end())
        return;

    if (eventMask & EV_READ)  i->second.cb_read = 0;
    if (eventMask & EV_PRIO)  i->second.cb_prio = 0;
    if (eventMask & EV_WRITE) i->second.cb_write = 0;

    epoll_event ev;
    memset(&ev,0,sizeof(ev));
    ev.events = i->second.epollMask();
    ev.data.fd = fd;

    int action (EPOLL_CTL_MOD);
    bool file (i->second.file);
    if (ev.events==0) {
        action = EPOLL_CTL_DEL;
        fdErase_.push_back(fd);
    }

    if (! file && epoll_ctl(epollFd_, action, fd, &ev) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("::epoll_ctl()");
    if (file)
        -- files_;
}

prefix_ void senf::Scheduler::registerSigHandlers()
{
    for (unsigned signal (1); signal < sigHandlers_.size(); ++signal) {
        if (sigHandlers_[signal]) {
            struct ::sigaction sa;
            sa.sa_sigaction = & Scheduler::sigHandler;
            sa.sa_mask = sigset_;
            sa.sa_flags = SA_SIGINFO;
            if (signal == SIGCHLD)
                sa.sa_flags |= SA_NOCLDSTOP;
            if (::sigaction(signal, &sa, 0) < 0)
                SENF_THROW_SYSTEM_EXCEPTION("");
        }
    }
}

prefix_ void senf::Scheduler::sigHandler(int signal, ::siginfo_t * siginfo, void *)
{
    // This is a bit unsafe. Better write single bytes and place the siginfo into an explicit
    // queue. Since signals are only unblocked during epoll_wait, we even wouldn't need to
    // synchronize access to that queue any further.

    ::write(instance().sigpipe_[1], siginfo, sizeof(*siginfo));

    // We ignore errors. The file handle is set to non-blocking IO. If any failure occurs (pipe
    // full), the signal will be dropped. That's like kernel signal handling which may also drop
    // signals.
}

prefix_ int senf::Scheduler::EventSpec::epollMask()
    const
{
    int mask (0);
    if (cb_read)  mask |= EPOLLIN;
    if (cb_prio)  mask |= EPOLLPRI;
    if (cb_write) mask |= EPOLLOUT;
    return mask;
}

prefix_ void senf::Scheduler::process()
{
    terminate_ = false;
    eventTime_ = ClockService::now();
    while (! terminate_) {

        // Since a callback may have disabled further timers, we need to check for canceled timeouts
        // again.

        while (! timerQueue_.empty()) {
            TimerMap::iterator i (timerQueue_.top());
            if (! i->second.canceled)
                break;
            timerMap_.erase(i);
            timerQueue_.pop();
        }

        for (FdEraseList::iterator i (fdErase_.begin()); i != fdErase_.end(); ++i) 
            fdTable_.erase(*i);
        fdErase_.clear();

        int timeout (-1);
        if (files_ > 0)
            timeout = 0;
        else {
            if (timerQueue_.empty()) {
                if (fdTable_.empty())
                    break;
            }
            else {
                ClockService::clock_type delta (
                    (timerQueue_.top()->second.timeout - eventTime_ + eventAdjust_)/1000000UL);
                timeout = delta < 0 ? 0 : delta;
            }
        }

        ///\todo Handle more than one epoll_event per call
        struct epoll_event ev;
        
        ::sigprocmask(SIG_UNBLOCK, &sigset_, 0);
        int events (epoll_wait(epollFd_, &ev, 1, timeout));
        ::sigprocmask(SIG_BLOCK, &sigset_, 0);

        if (events<0)
            if (errno != EINTR)
                SENF_THROW_SYSTEM_EXCEPTION("");

        eventTime_ = ClockService::now();

        // We always run timeout handlers. This is important, even if a file-descriptor is signaled
        // since some descriptors (e.g. real files) will *always* be ready and we still may want to
        // handle timers.  Time handlers are run before file events to not delay them unnecessarily.

        while (! timerQueue_.empty()) {
            TimerMap::iterator i (timerQueue_.top());
            if (i->second.canceled)
                ;
            else if (i->second.timeout <= eventTime_ + eventEarly_)
                i->second.cb();
            else
                break;
            timerQueue_.pop();
            timerMap_.erase(i);
        }

        // Check the signal queue
        if (events > 0 && ev.data.fd == sigpipe_[0]) {
            ::siginfo_t siginfo;
            if (::read(sigpipe_[0], &siginfo, sizeof(siginfo)) < int(sizeof(siginfo))) {
                // We ignore truncated records which may only occur if the signal
                // queue became filled up
                SENF_LOG((senf::log::IMPORTANT)("Truncated signal record!"));
                continue;
            }
            if (siginfo.si_signo < int(sigHandlers_.size()) && sigHandlers_[siginfo.si_signo])
                sigHandlers_[siginfo.si_signo]();
            continue;
        }

        for (FdTable::iterator i = fdTable_.begin(); i != fdTable_.end(); ++i) {
            EventSpec & spec (i->second);

            if (! (spec.file || (events > 0 && i->first == ev.data.fd)))
                continue;
                
            unsigned extraFlags (0);
            unsigned mask (spec.file ? spec.epollMask() : ev.events);

            if (mask & EPOLLHUP) extraFlags |= EV_HUP;
            if (mask & EPOLLERR) extraFlags |= EV_ERR;

            if (mask & EPOLLIN) {
                SENF_ASSERT(spec.cb_read);
                spec.cb_read(EventId(EV_READ | extraFlags));
            }
            else if (mask & EPOLLPRI) {
                SENF_ASSERT(spec.cb_prio);
                spec.cb_prio(EventId(EV_PRIO | extraFlags));
            }
            else if (mask & EPOLLOUT) {
                SENF_ASSERT(spec.cb_write);
                spec.cb_write(EventId(EV_WRITE | extraFlags));
            }
            else {
                // This branch is only taken, if HUP or ERR is signaled but none of IN/OUT/PRI. 
                // In this case we will signal all registered callbacks. The callbacks must be
                // prepared to be called multiple times if they are registered to more than
                // one event.
                if (spec.cb_write) 
                    spec.cb_write(EventId(extraFlags));
                if (spec.cb_prio) 
                    spec.cb_prio(EventId(extraFlags));
                if (spec.cb_read) 
                    spec.cb_read(EventId(extraFlags));
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////
// senf::SchedulerLogTimeSource

prefix_ boost::posix_time::ptime senf::SchedulerLogTimeSource::operator()()
    const
{
    return ClockService::abstime(Scheduler::instance().eventTime());
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
