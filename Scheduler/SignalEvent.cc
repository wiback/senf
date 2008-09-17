// $Id$
//
// Copyright (C) 2008 
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
    \brief SignalDispatcher non-inline non-template implementation */

#include "SignalEvent.hh"
#include "SignalEvent.ih"

// Custom includes
#include "../Utils/senfassert.hh"
#include "../Utils/signalnames.hh"

//#include "SignalEvent.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::scheduler::detail::SignalDispatcher::SignalDispatcher()
    : blocked_ (true)
{
    if (pipe(sigPipe_) <0)
        SENF_THROW_SYSTEM_EXCEPTION("pipe()");
    sigemptyset(&sigSet_);
    FdManager::instance().set(sigPipe_[0], FdManager::EV_READ, this);
}

prefix_ senf::scheduler::detail::SignalDispatcher::~SignalDispatcher()
{
    for (SignalSet::iterator i (handlers_.begin()); i != handlers_.end(); ++i) {
        ::signal(i->signal_, SIG_DFL);
        FIFORunner::instance().dequeue(&(*i));
    }
    sigprocmask(SIG_UNBLOCK, &sigSet_, 0);
    FdManager::instance().remove(sigPipe_[0]);
    close(sigPipe_[0]);
    close(sigPipe_[1]);
}

prefix_ void senf::scheduler::detail::SignalDispatcher::add(SignalEvent & event)
{
    SignalSet::iterator i (handlers_.find(event));
    if (i != handlers_.end())
        throw DuplicateSignalRegistrationException() 
            << " for signal " << signalName(event.signal_) << " (" << event.signal_ << ")";

    handlers_.insert(event);
    sigaddset(&sigSet_, event.signal_);
    FIFORunner::instance().enqueue(&event);

    sigset_t sig;
    sigemptyset(&sig);
    sigaddset(&sig, event.signal_);
    sigprocmask(blocked_ ? SIG_BLOCK : SIG_UNBLOCK, &sig, 0);

    // Need to re-register all handlers to update sa_mask
    struct sigaction act;
    act.sa_sigaction = &sigHandler;
    act.sa_mask = sigSet_;
    act.sa_flags = SA_SIGINFO | SA_RESTART;
    for (SignalSet::iterator i (handlers_.begin()); i != handlers_.end(); ++i) {
        if (i->signal_ == SIGCLD)
            act.sa_flags |= SA_NOCLDSTOP;
        else
            act.sa_flags &= ~SA_NOCLDSTOP;
        if (sigaction(i->signal_, &act, 0) < 0)
            SENF_THROW_SYSTEM_EXCEPTION("sigaction()");
    }
}

prefix_ void senf::scheduler::detail::SignalDispatcher::remove(SignalEvent & event)
{
    ::signal(event.signal_, SIG_DFL);
    FIFORunner::instance().dequeue(&event);
    handlers_.erase(event);
    sigset_t sig;
    sigemptyset(&sig);
    sigaddset(&sig, event.signal_);
    sigprocmask(SIG_UNBLOCK, &sig, 0);
}

prefix_ void senf::scheduler::detail::SignalDispatcher::signal(int events)
{
    siginfo_t info;
    if (read(sigPipe_[0], &info, sizeof(info)) < int(sizeof(info)))
        return;
    SignalSet::iterator i (handlers_.find(info.si_signo, FindNumericSignal()));
    if (i == handlers_.end())
        return;
    i->siginfo_ = info;
    i->runnable = true;
}

prefix_ void senf::scheduler::detail::SignalDispatcher::sigHandler(int signal, ::siginfo_t * siginfo,
                                                           void *)
{
    SENF_ASSERT( alive() );
    // The manpage says, si_signo is unused in linux so we set it here
    siginfo->si_signo = signal; 
    // We can't do much on error anyway so we ignore errors here
    write(instance().sigPipe_[1], siginfo, sizeof(*siginfo));
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "SignalEvent.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
