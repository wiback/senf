// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

    \idea Implement signal handling (See source for more discussion
    about this)

    \idea Multithreading support: To support multithreading, the
    static member Scheduler::instance() must return a thread-local
    value (that is Scheduler::instance() must allocate one Scheduler
    instance per thread). Another possibility would be to distribute
    the async load unto several threads (one scheduler for multiple
    threads)
 */

// Here a basic concept of how to add signal support to the scheduler:
//
// Every signal to be reported by the scheduler will be asigned a
// generic signal handler by the scheduler. This signal handler will
// use longjmp (juck) to report this signal back to the scheduler
// main-loop.
//
// To make this safe, the main-loop will look something like:
//
//     int signal = setjmp(jmpBuffer_);
//     if (signal == 0) {
//         // unblock all signals which are registered with the
//         // scheduler
//         // call epoll
//         // block all relevant signals again
//     }
//
//     // now handle the event
//
// The signal handler is then simply defined as
//
//     static void Scheduler::sigHandler(int signal)
//     {
//         // make sure to restore the signal handler here if
//         // necessary
//         longjmp(Scheduler::instance().jmpBuffer_,signal);
//     }
//
// You should use sigaction to register the signal handlers and define
// a sa_mask so all Scheduler-registered signals are automatically
// *blocked* whenever one of the signals is called (including the
// called signal!) (This also means, we will have to re-register all
// signals if we change the registration of some signal since the
// sa_mask changes). This ensures, that no two signals can be
// delivered on top of each other. And of course any signal registered
// with the scheduler must be blocked as soon as it is registered with
// the scheduler.

#include "Scheduler.hh"
//#include "Scheduler.ih"

// Custom includes
#include <errno.h>
#include <sys/epoll.h>
#include "Utils/Exception.hh"
#include "Utils/MicroTime.hh"

static const int EPollInitialSize = 16;

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::Scheduler::Scheduler & senf::Scheduler::instance()
{
    static Scheduler instance;
    return instance;
}

prefix_ void senf::Scheduler::timeout(unsigned long timeout, TimerCallback const & cb)
{
    timerQueue_.push(TimerSpec(now()+1000*timeout,cb));
}

prefix_ senf::Scheduler::Scheduler()
    : epollFd_(epoll_create(EPollInitialSize))
{
    if (epollFd_<0)
        throw SystemException(errno);
}

prefix_ void senf::Scheduler::do_add(int fd, SimpleCallback const & cb, int eventMask)
{
    FdTable::iterator i (fdTable_.find(fd));
    int action (EPOLL_CTL_MOD);
    if (i == fdTable_.end()) {
        action = EPOLL_CTL_ADD;
        i = fdTable_.insert(std::make_pair(fd, EventSpec())).first;
    }

    if (eventMask & EV_READ)  i->second.cb_read = cb;
    if (eventMask & EV_PRIO)  i->second.cb_prio = cb;
    if (eventMask & EV_WRITE) i->second.cb_write = cb;
    if (eventMask & EV_HUP)   i->second.cb_hup = cb;
    if (eventMask & EV_ERR)   i->second.cb_err = cb;

    epoll_event ev;
    memset(&ev,0,sizeof(ev));
    ev.events = i->second.epollMask();
    ev.data.fd = fd;

    if (epoll_ctl(epollFd_, action, fd, &ev)<0)
        throw SystemException(errno);
}

prefix_ void senf::Scheduler::do_remove(int fd, int eventMask)
{
    FdTable::iterator i (fdTable_.find(fd));
    if (i == fdTable_.end())
        return;

    if (eventMask & EV_READ)  i->second.cb_read = 0;
    if (eventMask & EV_PRIO)  i->second.cb_prio = 0;
    if (eventMask & EV_WRITE) i->second.cb_write = 0;
    if (eventMask & EV_HUP)   i->second.cb_hup = 0;
    if (eventMask & EV_ERR)   i->second.cb_err = 0;

    epoll_event ev;
    memset(&ev,0,sizeof(ev));
    ev.events = i->second.epollMask();
    ev.data.fd = fd;

    int action (EPOLL_CTL_MOD);
    if (ev.events==0) {
        action = EPOLL_CTL_DEL;
        fdTable_.erase(i);
    }

    if (epoll_ctl(epollFd_, action, fd, &ev)<0)
        throw SystemException(errno);
}


prefix_ int senf::Scheduler::EventSpec::epollMask()
    const
{
    int mask (0);
    if (cb_read)  mask |= EPOLLIN;
    if (cb_prio)  mask |= EPOLLPRI;
    if (cb_write) mask |= EPOLLOUT;
    if (cb_hup)   mask |= EPOLLHUP;
    if (cb_err)   mask |= EPOLLERR;
    return mask;
}

prefix_ void senf::Scheduler::process()
{
    terminate_ = false;
    while (! terminate_) {

        MicroTime timeNow = now();
        while ( ! timerQueue_.empty() && timerQueue_.top().timeout <= timeNow ) {
            timerQueue_.top().cb();
            timerQueue_.pop();
        }
        if (terminate_)
            return;
        int timeout = timerQueue_.empty() ? -1 : int((timerQueue_.top().timeout - timeNow)/1000);

        struct epoll_event ev;
        int events = epoll_wait(epollFd_, &ev, 1, timeout);
        if (events<0)
            // Hmm ... man epoll says, it will NOT return with EINTR ??
            throw SystemException(errno);
        if (events==0)
            // Timeout .. it will be run when reachiung the top of the loop
            continue;

        FdTable::iterator i = fdTable_.find(ev.data.fd);
        BOOST_ASSERT (i != fdTable_.end() );
        EventSpec const & spec (i->second);

        if (ev.events & EPOLLIN) {
            BOOST_ASSERT(spec.cb_read);
            spec.cb_read(EV_READ);
        }
        else if (ev.events & EPOLLPRI) {
            BOOST_ASSERT(spec.cb_prio);
            spec.cb_prio(EV_PRIO);
        }
        else if (ev.events & EPOLLOUT) {
            BOOST_ASSERT(spec.cb_write);
            spec.cb_write(EV_WRITE);
        }

        else if (ev.events & EPOLLHUP) {
            if (spec.cb_hup)
                spec.cb_hup(EV_HUP);
            else if (ev.events & EPOLLERR) {
                /** \fixme This is stupid, if cb_write and cb_read are
                    the same. The same below. We really have to
                    exactly define sane semantics of what to do on
                    EPOLLHUP and EPOLLERR. */
                if (spec.cb_write) spec.cb_write(EV_HUP);
                if (spec.cb_read) spec.cb_read(EV_HUP);
            }
        }
        else if (ev.events & EPOLLERR && ! ev.events & EPOLLHUP) {
            if (spec.cb_err)
                spec.cb_err(EV_ERR);
            else {
                if (spec.cb_write) spec.cb_write(EV_ERR);
                if (spec.cb_read) spec.cb_read(EV_ERR);
            }
        }

    }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
