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
    \brief FIFORunner public header */

#ifndef HH_FIFORunner_
#define HH_FIFORunner_ 1

// Custom includes
#include <signal.h>
#include <boost/utility.hpp>
#include "../boost/intrusive/ilist.hpp"
#include "../boost/intrusive/ilist_hook.hpp"
#include "../Utils/singleton.hh"

//#include "FIFORunner.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf { 

    class Scheduler;

namespace scheduler {

    /** \brief Task execution scheduler

        The FIFORunner implements a simple FIFO scheduler for callback tasks. All tasks are held in
        a queue. Whenever a task is run, it is moved to the end of the queue. Running the queue will
        run all tasks which have been marked runnable. 

        When running a task, it's runnable flag is always reset. The flag is set whenever an event
        is posted for the task.
      */
    class FIFORunner
        : public singleton<FIFORunner>
    {
    public:
        struct TaskInfo;

    private:
        struct TaskListTag;
        typedef boost::intrusive::ilist_base_hook<TaskListTag> TaskListBase;
        typedef boost::intrusive::ilist<TaskListBase::value_traits<TaskInfo>, false> TaskList;

    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        /** \brief Task structure

            TaskInfo is the base-class for all tasks.
         */
        struct TaskInfo 
            : public TaskListBase
        {
            TaskInfo();
            virtual ~TaskInfo();

            bool runnable;              ///< Runnable flag
                                        /**< This must be set to \c true when the task is
                                             runnable. It is reset automatically when the task is
                                             run. */

            std::string name;           ///< Descriptive task name
#       ifdef SENF_DEBUG
            std::string backtrace;
#       endif
            virtual void run() = 0;     ///< Called to run the task
        };

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        using singleton<FIFORunner>::instance;
        using singleton<FIFORunner>::alive;

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        void enqueue(TaskInfo * task);  ///< Add task to queue
        void dequeue(TaskInfo * task);  ///< Remove task from queue
        
        void run();                     ///< Run queue

        void taskTimeout(unsigned ms);  ///< Set task timeout to \a ms milliseconds
        unsigned taskTimeout() const;   ///< Get task timeout in milliseconds

        unsigned hangCount() const;     ///< Number of task expirations
                                        /**< The FIFORunner manages a watchdog which checks, that a
                                             single task does not run continuously for a longer time
                                             or block. If a task runs for more than 1s, a warning is
                                             printed  and the hangCount is increased. */

    protected:

    private:
        FIFORunner();
        ~FIFORunner();

        static void watchdog(int, siginfo_t *, void *);

        TaskList tasks_;
        TaskList::iterator next_;
        timer_t watchdogId_;
        unsigned watchdogMs_;
        std::string runningName_;
#   ifdef SENF_DEBUG
        std::string runningBacktrace_;
#   endif
        unsigned watchdogCount_;
        unsigned hangCount_;

        friend class singleton<FIFORunner>;
        friend class senf::Scheduler;
    };


}}

///////////////////////////////hh.e////////////////////////////////////////
#include "FIFORunner.cci"
//#include "FIFORunner.ct"
//#include "FIFORunner.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
