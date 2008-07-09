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

#include "SignalDispatcher.hh"
//#include "SignalDispatcher.ih"

// Custom includes
#include "../Utils/senfassert.hh"

//#include "SignalDispatcher.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

senf::scheduler::SignalDispatcher * senf::scheduler::SignalDispatcher::instance_ = 0;

prefix_ senf::scheduler::SignalDispatcher::SignalDispatcher(FdManager & manager,
                                                            FIFORunner & runner)
    : manager_ (manager), runner_ (runner), blocked_ (true)
{
    SENF_ASSERT( !instance_ );
    if (pipe(sigPipe_) <0)
        SENF_THROW_SYSTEM_EXCEPTION("pipe()");
    sigemptyset(&sigSet_);
    instance_ = this;
    manager_.set(sigPipe_[0], FdManager::EV_READ, this);
}

prefix_ senf::scheduler::SignalDispatcher::~SignalDispatcher()
{
    for (HandlerMap::iterator i (handlers_.begin()); i != handlers_.end(); ++i) {
        ::signal(i->first, SIG_DFL);
        runner_.dequeue(&i->second);
    }
    sigprocmask(SIG_UNBLOCK, &sigSet_, 0);
    manager_.remove(sigPipe_[0]);
    close(sigPipe_[0]);
    close(sigPipe_[1]);
    instance_ = 0;
}

prefix_ void senf::scheduler::SignalDispatcher::add(int signal, Callback const & cb)
{
    HandlerMap::iterator i (handlers_.find(signal));
    if (i != handlers_.end()) {
        i->second.cb = cb;
        return;
    }

    i = handlers_.insert(std::make_pair(signal, SignalEvent(signal, cb))).first;
    sigaddset(&sigSet_, signal);
    runner_.enqueue(&i->second);

    sigset_t sig;
    sigemptyset(&sig);
    sigaddset(&sig, signal);
    sigprocmask(blocked_ ? SIG_BLOCK : SIG_UNBLOCK, &sig, 0);

    // Need to re-register all handlers to update sa_mask
    for (HandlerMap::iterator j (handlers_.begin()); i != handlers_.end(); ++i) {
        struct sigaction act;
        act.sa_sigaction = &sigHandler;
        act.sa_mask = sigSet_;
        act.sa_flags = SA_SIGINFO | SA_RESTART;
        if (j->first == SIGCLD)
            act.sa_flags |= SA_NOCLDSTOP;
        if (sigaction(j->first, &act, 0) < 0)
            SENF_THROW_SYSTEM_EXCEPTION("sigaction()");
    }
}

prefix_ void senf::scheduler::SignalDispatcher::remove(int signal)
{
    ::signal(signal, SIG_DFL);
    sigset_t sig;
    sigemptyset(&sig);
    sigaddset(&sig, signal);
    sigprocmask(SIG_UNBLOCK, &sig, 0);
}

prefix_ void senf::scheduler::SignalDispatcher::signal(int events)
{
    siginfo_t info;
    if (read(sigPipe_[0], &info, sizeof(info)) < int(sizeof(info)))
        return;
    HandlerMap::iterator i (handlers_.find(info.si_signo));
    if (i == handlers_.end())
        return;
    i->second.siginfo = info;
    i->second.runnable = true;
}

prefix_ void senf::scheduler::SignalDispatcher::sigHandler(int signal, ::siginfo_t * siginfo,
                                                           void *)
{
    SENF_ASSERT( instance_ );
    // The manpage says, si_signo is unused in linux so we set it here
    siginfo->si_signo = signal; 
    // We can't do much on error anyway so we ignore errors here
    write(instance_->sigPipe_[1], siginfo, sizeof(*siginfo));
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "SignalDispatcher.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
