// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief SignalDispatcher non-inline non-template implementation */

#include "SignalEvent.hh"
#include "SignalEvent.ih"

// Custom includes
#include <senf/Utils/senfassert.hh>
#include <senf/Utils/signalnames.hh>
#include "senf/Utils/IgnoreValue.hh"

//#include "SignalEvent.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::scheduler::detail::SignalDispatcher::SignalDispatcher()
    : blocked_ (true)
{
    if (pipe(sigPipe_) <0)
        SENF_THROW_SYSTEM_EXCEPTION("pipe()");
    sigemptyset(&sigSet_);
    detail::FdManager::instance().set(sigPipe_[0], detail::FdManager::EV_READ, this);
}

prefix_ senf::scheduler::detail::SignalDispatcher::~SignalDispatcher()
{
    for (SignalSet::iterator i (handlers_.begin()); i != handlers_.end(); ++i) {
        ::signal(i->signal_, SIG_DFL);
        detail::FIFORunner::instance().dequeue(&(*i));
    }
    sigprocmask(SIG_UNBLOCK, &sigSet_, 0);
    detail::FdManager::instance().remove(sigPipe_[0]);
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
    detail::FIFORunner::instance().enqueue(&event);

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
    detail::FIFORunner::instance().dequeue(&event);
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
    i->setRunnable();
}

prefix_ void senf::scheduler::detail::SignalDispatcher::sigHandler(int signal,
                                                                   ::siginfo_t * siginfo,
                                                                   void *)
{
    SENF_ASSERT( alive(), "Internal failure: Destroyed signal handler called" );
    // The manpage says, si_signo is unused in linux so we set it here
    siginfo->si_signo = signal;
    // We can't do much on error anyway so we ignore errors here
    senf::IGNORE( write(instance().sigPipe_[1], siginfo, sizeof(*siginfo)) );
}

prefix_ void senf::scheduler::SignalEvent::v_run()
{
    cb_(siginfo_);
}

prefix_ char const * senf::scheduler::SignalEvent::v_type()
    const
{
    return "si";
}

prefix_ std::string senf::scheduler::SignalEvent::v_info()
    const
{
    return "";
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
