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

// At the moment, the FIFORunner is not very efficient with many non-runnable tasks since the
// complete list of tasks is traversed on each run().
//
// To optimize this, we woould need a way to find the relative ordering of two tasks in O(1) (at the
// moment, this is an O)(N) operation by traversing the list).
//
// One idea is, to give each task an 'order' value. Whenever a task is added at the end, it's order
// value is set to the order value of the last task + 1. Whenever the order value such added exceeds
// some threshold (e.g. 2^31 -1 or some such), the task list is traversed from beginning to end to
// assign new consecutive order values. This O(N) operation is so seldom, that it is amortized over
// a very long time.
//
// With this value at hand, we can do several optimizations: One idea would be the following: The
// runnable set always has two types of tasks: There are tasks, which are heavily active and are
// signaled constantly and other tasks which lie dormant most of the time. Those dormant tasks will
// end up at the beginning of the task queue.
//
// With the above defined 'ordering' field available, we can manage an iterator pointing to the
// first and the last runnable task. This will often help a lot since the group of runnable tasks
// will mostly be localized to the end of the queue. only occasionally one of the dormant tasks will
// be runnable. This additional traversal time will be amortized over a larger time.

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
