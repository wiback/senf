//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief FdDispatcher non-inline non-template implementation */

#include "FdEvent.hh"
#include "FdEvent.ih"

// Custom includes
#include <sstream>

//#include "FdEvent.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
    fds_.erase(FdSet::s_iterator_to(event));
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
    fds_.erase(FdSet::s_iterator_to(event));
    detail::FIFORunner::instance().dequeue(&event);
    if (fds_.empty())
        detail::FdManager::instance().timeout(managerTimeout_);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
        enable();
    return *this;
}

prefix_ void senf::scheduler::FdEvent::signal(int events)
{
    detail::FdDispatcher::FdSet::iterator i (detail::FdDispatcher::FdSet::s_iterator_to(*this));
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
