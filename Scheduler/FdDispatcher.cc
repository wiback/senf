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

#include "FdDispatcher.hh"
#include "FdDispatcher.ih"

// Custom includes
#include "../Utils/senfassert.hh"

//#include "FdDispatcher.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::scheduler::FdDispatcher

prefix_ senf::scheduler::FdDispatcher::~FdDispatcher()
{
    for (FdMap::iterator i (fds_.begin()); i != fds_.end(); ++i) {
        manager_.remove(i->first);
        runner_.dequeue(static_cast<FdEvent::ReadTask*>(&i->second));
        runner_.dequeue(static_cast<FdEvent::PrioTask*>(&i->second));
        runner_.dequeue(static_cast<FdEvent::WriteTask*>(&i->second));
    }
}

prefix_ void senf::scheduler::FdDispatcher::add(int fd, Callback const & cb, int events)
{
    if (events == 0)
        return;
    
    FdMap::iterator i (fds_.find(fd));
    if (i == fds_.end()) {
        i = fds_.insert(std::make_pair(fd, FdEvent())).first;
        runner_.enqueue(static_cast<FdEvent::ReadTask*>(&i->second));
        runner_.enqueue(static_cast<FdEvent::PrioTask*>(&i->second));
        runner_.enqueue(static_cast<FdEvent::WriteTask*>(&i->second));
    }
    FdEvent & event (i->second);

    if (events & EV_READ) event.FdEvent::ReadTask::cb = cb;
    if (events & EV_PRIO) event.FdEvent::PrioTask::cb = cb;
    if (events & EV_WRITE) event.FdEvent::WriteTask::cb = cb;

    manager_.set(fd, event.activeEvents(), &event);
}

prefix_ void senf::scheduler::FdDispatcher::remove(int fd, int events)
{
    if (events == 0)
        return;

    FdMap::iterator i (fds_.find(fd));
    if (i == fds_.end())
        return;
    FdEvent & event (i->second);

    if (events & EV_READ) event.FdEvent::ReadTask::cb = 0;
    if (events & EV_PRIO) event.FdEvent::PrioTask::cb = 0;
    if (events & EV_WRITE) event.FdEvent::WriteTask::cb = 0;

    int activeEvents (event.activeEvents());
    if (! activeEvents) {
        manager_.remove(fd);
        runner_.dequeue(static_cast<FdEvent::ReadTask*>(&i->second));
        runner_.dequeue(static_cast<FdEvent::PrioTask*>(&i->second));
        runner_.dequeue(static_cast<FdEvent::WriteTask*>(&i->second));
        fds_.erase(fd);
    } else
        manager_.set(fd, activeEvents, &event);
}

///////////////////////////////////////////////////////////////////////////
// senf::scheduler::FdDispatcher::FdEvent

prefix_ void senf::scheduler::FdDispatcher::FdEvent::signal(int e)
{
    events = e;

    if (events & EV_READ) 
        ReadTask::runnable = true;
    if (events & EV_PRIO) 
        PrioTask::runnable = true;
    if (events & EV_WRITE)
        WriteTask::runnable = true;

    if (events & (EV_ERR | EV_HUP) && ! events & (EV_READ | EV_PRIO | EV_WRITE)) {
        if (ReadTask::cb) ReadTask::runnable = true;
        if (PrioTask::cb) PrioTask::runnable = true;
        if (WriteTask::cb) WriteTask::runnable = true;
    }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "FdDispatcher.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
