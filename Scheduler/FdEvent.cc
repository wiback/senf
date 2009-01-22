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
    \brief FdDispatcher non-inline non-template implementation */

#include "FdEvent.hh"
#include "FdEvent.ih"

// Custom includes
#include <sstream>
#include "../Utils/senfassert.hh"

//#include "FdEvent.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::scheduler::detail::FdDispatcher

prefix_ senf::scheduler::detail::FdDispatcher::~FdDispatcher()
{
    for (FdSet::iterator i (fds_.begin()); i != fds_.end(); ++i) {
        detail::FdManager::instance().remove(i->fd_);
        detail::FIFORunner::instance().dequeue(&(*i));
    }
}

prefix_ bool senf::scheduler::detail::FdDispatcher::add(FdEvent & event)
{
    std::pair<FdSet::iterator,FdSet::iterator> range (fds_.equal_range(event));
    int events (0);
    for (FdSet::iterator i (range.first); i != range.second; ++i)
        events |= i->events_;
    if (event.events_ & events)
        throw FdEvent::DuplicateEventRegistrationException() << "vinfo: " << event.v_info();

    if (! detail::FdManager::instance().set(event.fd_, events | event.events_, &event))
        return false;

    detail::FIFORunner::instance().enqueue(&event);
    fds_.insert(range.first, event);

    return true;
}

prefix_ void senf::scheduler::detail::FdDispatcher::remove(FdEvent & event)
{
    fds_.erase(FdSet::current(event));
    detail::FIFORunner::instance().dequeue(&event);

    std::pair<FdSet::iterator,FdSet::iterator> range (fds_.equal_range(event));
    if (range.first == range.second)
        detail::FdManager::instance().remove(event.fd_);
    else {
        int events (0);
        for (FdSet::iterator i (range.first); i != range.second; ++i)
            events |= i->events_;
        detail::FdManager::instance().set(event.fd_, events, &(*range.first));
    }
}

///////////////////////////////////////////////////////////////////////////
// senf::scheduler::detail::FileDispatcher

prefix_ void senf::scheduler::detail::FileDispatcher::add(FdEvent & event)
{
    std::pair<FdSet::iterator,FdSet::iterator> range (fds_.equal_range(event));
    int events (0);
    for (FdSet::iterator i (range.first); i != range.second; ++i)
        events |= i->events_;
    if (event.events_ & events)
        throw FdEvent::DuplicateEventRegistrationException() << "vinfo: " << event.v_info();

    detail::FIFORunner::instance().enqueue(&event);
    fds_.insert(range.first, event);

    detail::FdManager::instance().timeout(0);
}

prefix_ senf::scheduler::detail::FileDispatcher::FileDispatcher()
    : managerTimeout_ (scheduler::detail::FdManager::instance().timeout())
{}

prefix_ senf::scheduler::detail::FileDispatcher::~FileDispatcher()
{
    detail::FdManager::instance().timeout(-1);
    for (FdSet::iterator i (fds_.begin()); i != fds_.end(); ++i)
        detail::FIFORunner::instance().dequeue(&(*i));
}

prefix_ void senf::scheduler::detail::FileDispatcher::prepareRun()
{
    for (FdSet::iterator i (fds_.begin()); i != fds_.end(); ++i) {
        i->signaledEvents_ = i->events_;
        i->setRunnable();
    }
}

prefix_ void senf::scheduler::detail::FileDispatcher::remove(FdEvent & event)
{
    fds_.erase(FdSet::current(event));
    detail::FIFORunner::instance().dequeue(&event);
    if (fds_.empty())
        detail::FdManager::instance().timeout(managerTimeout_);
}

///////////////////////////////////////////////////////////////////////////
// senf::scheduler::FdEvent

prefix_ void senf::scheduler::FdEvent::disable()
{
    if (enabled()) {
        if (pollable_)
            detail::FdDispatcher::instance().remove(*this);
        else
            detail::FileDispatcher::instance().remove(*this);
    }
}

prefix_ void senf::scheduler::FdEvent::enable()
{
    if (! enabled() && events_ && fd_ != -1) {
        if (pollable_ && detail::FdDispatcher::instance().add(*this))
            return;
        detail::FileDispatcher::instance().add(*this);
        pollable_ = false;
    }
}

prefix_ senf::scheduler::FdEvent & senf::scheduler::FdEvent::events(int events)
{
    bool en = enabled();
    disable();
    events_ = events;
    if (en)
        enabled();
    return *this;
}

prefix_ void senf::scheduler::FdEvent::signal(int events)
{
    detail::FdDispatcher::FdSet::iterator i (detail::FdDispatcher::FdSet::current(*this));
    detail::FdDispatcher::FdSet::iterator const i_end (detail::FdDispatcher::instance().fds_.end());
    bool all ((events & (EV_ERR | EV_HUP)) && ! (events & (EV_READ | EV_PRIO | EV_WRITE)));
    for (; i != i_end && fd_ == i->fd_; ++i) {
        i->signaledEvents_ = events & (EV_ERR | EV_HUP | i->events_);
        if (i->events_ & events || all)
            i->setRunnable();
    }
}

prefix_ void senf::scheduler::FdEvent::v_run()
{
    cb_(signaledEvents_);
}

prefix_ char const * senf::scheduler::FdEvent::v_type()
    const
{
    return "fd";
}

prefix_ std::string senf::scheduler::FdEvent::v_info()
    const
{
    std::stringstream ss;

    ss << "fd " << fd_;
    if (! pollable_)
        ss << " (NOT pollable)";
    if (events_ & EV_READ)
        ss << " READ";
    if (events_ & EV_PRIO)
        ss << " PRIO";
    if (events_ & EV_WRITE)
        ss << " WRITE";
    return ss.str();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "FdEvent.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
