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
    \brief FileDispatcher non-inline non-template implementation */

#include "FileDispatcher.hh"
//#include "FileDispatcher.ih"

// Custom includes

//#include "FileDispatcher.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::scheduler::FileDispatcher::FileDispatcher(FdManager & manager,
                                                        FIFORunner & runner)
    : manager_ (manager), runner_ (runner), managerTimeout_ (manager.timeout())
{}

prefix_ senf::scheduler::FileDispatcher::~FileDispatcher()
{
    manager_.timeout(-1);

    for (FileMap::iterator i (files_.begin()); i != files_.end(); ++i) {
        runner_.dequeue(static_cast<FileEvent::ReadTask*>(&i->second));
        runner_.dequeue(static_cast<FileEvent::WriteTask*>(&i->second));
    }
}

prefix_ void senf::scheduler::FileDispatcher::add(std::string const & name, int fd,
                                                  Callback const & cb, int events)
{
    if (events == 0)
        return;
    
    FileMap::iterator i (files_.find(fd));
    if (i == files_.end()) {
        i = files_.insert(std::make_pair(fd, FileEvent(name))).first;
        runner_.enqueue(static_cast<FileEvent::ReadTask*>(&i->second));
        runner_.enqueue(static_cast<FileEvent::WriteTask*>(&i->second));
    }
    FileEvent & event (i->second);

    if (events & EV_READ)
        event.FileEvent::ReadTask::cb = cb;
    if (events & EV_WRITE)
        event.FileEvent::WriteTask::cb = cb;
    
    manager_.timeout(0);
}

prefix_ void senf::scheduler::FileDispatcher::remove(int fd, int events)
{
    if (events == 0)
        return;

    FileMap::iterator i (files_.find(fd));
    if (i == files_.end()) 
        return;
    FileEvent & event (i->second);

    if (events & EV_READ) event.FileEvent::ReadTask::cb = 0;
    if (events & EV_WRITE) event.FileEvent::WriteTask::cb = 0;

    int activeEvents (event.activeEvents());
    if (! activeEvents) {
        runner_.dequeue(static_cast<FileEvent::ReadTask*>(&i->second));
        runner_.dequeue(static_cast<FileEvent::WriteTask*>(&i->second));
        files_.erase(fd);
    }
    
    if (files_.empty())
        manager_.timeout(managerTimeout_);
}

prefix_ void senf::scheduler::FileDispatcher::prepareRun()
{
    for (FileMap::iterator i (files_.begin()); i != files_.end(); ++i) {
        i->second.events = i->second.activeEvents();
        if (i->second.FileEvent::ReadTask::cb)
            i->second.FileEvent::ReadTask::runnable = true;
        if (i->second.FileEvent::WriteTask::cb)
            i->second.FileEvent::WriteTask::runnable = true;
    }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "FileDispatcher.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
