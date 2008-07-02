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
    \brief FIFORunner non-inline non-template implementation */

#include "FIFORunner.hh"
//#include "FIFORunner.ih"

// Custom includes

//#include "FIFORunner.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ void senf::scheduler::FIFORunner::dequeue(TaskInfo * task)
{
    TaskList::iterator i (TaskList::current(*task));
    if (next_ == i)
        ++next_;
    tasks_.erase(i);
}

namespace {
    struct NullTask 
        : public senf::scheduler::FIFORunner::TaskInfo
    {
        void run() {};
    };
}

prefix_ void senf::scheduler::FIFORunner::run()
{
    // This algorithm is carefully adjusted to make it work even when arbitrary tasks are removed
    // from the queue
    // - Before we begin, we add a NullTask to the queue. The only purpose of this node is, to mark
    //   the current end of the queue. The iterator to this node becomes the end iterator of the
    //   range to process
    // - We update the TaskInfo and move it to the end of the queue before calling the callback so
    //   we don't access the TaskInfo if it is removed while the callback is running
    // - We keep the next to-be-processed node in a class variable which is checked and updated
    //   whenever a node is removed.
    NullTask null;
    tasks_.push_back(null);
    TaskList::iterator end (TaskList::current(null));
    next_ = tasks_.begin();
    while (next_ != end) {
        TaskInfo & task (*next_);
        if (task.runnable) {
            task.runnable = false;
            TaskList::iterator i (next_);
            ++ next_;
            tasks_.splice(tasks_.end(), tasks_, i);
            task.run();
        }
        else
            ++ next_;
    }
    tasks_.erase(end);
    next_ = tasks_.end();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "FIFORunner.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
