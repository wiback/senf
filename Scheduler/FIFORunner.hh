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

#ifndef HH_SENF_Scheduler_FIFORunner_
#define HH_SENF_Scheduler_FIFORunner_ 1

// Custom includes
#include <signal.h>
#include <boost/utility.hpp>
#include "../boost/intrusive/ilist.hpp"
#include "../boost/intrusive/ilist_hook.hpp"
#include "../Utils/singleton.hh"
#include "EventManager.hh"

//#include "FIFORunner.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf { 
namespace scheduler {

    void restart();

namespace detail {

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
        class TaskInfo 
            : public Event, 
              public TaskListBase
        {
        public:
            enum Priority { PRIORITY_LOW = 0, PRIORITY_NORMAL = 1, PRIORITY_HIGH = 2 };

            explicit TaskInfo(std::string const & name, Priority priority=PRIORITY_NORMAL);
            virtual ~TaskInfo();

            void run();

            bool runnable() const;

        protected:
            void setRunnable();
            
        private:
            virtual void v_run() = 0;
            virtual bool v_enabled() const;

            bool runnable_;
            Priority priority_;
#       ifdef SENF_DEBUG
            std::string backtrace_;
#       endif

            friend class FIFORunner;
        };

        typedef boost::filter_iterator<
            EventManager::IteratorFilter, TaskList::const_iterator> iterator;

        using singleton<FIFORunner>::instance;
        using singleton<FIFORunner>::alive;

        void enqueue(TaskInfo * task);
        void dequeue(TaskInfo * task);
        
        void run();

        void taskTimeout(unsigned ms);
        unsigned taskTimeout() const;
        void abortOnTimeout(bool flag);
        bool abortOnTimeout() const;

        void startWatchdog();
        void stopWatchdog();

        unsigned hangCount();

        iterator begin() const;
        iterator end() const;

        void yield();

    protected:

    private:
        FIFORunner();
        ~FIFORunner();

        static void watchdog(int, siginfo_t *, void *);

        TaskList::iterator priorityEnd(TaskInfo::Priority p);
        void run(TaskList::iterator f, TaskList::iterator l);
        
        struct NullTask : public TaskInfo
        {
            NullTask();
            ~NullTask();
            virtual void v_run();;
            virtual char const * v_type() const;
            virtual std::string v_info() const;
        };

        TaskList tasks_;
        TaskList::iterator next_;

        NullTask normalPriorityEnd_;
        NullTask highPriorityEnd_;
        
        timer_t watchdogId_;
        bool watchdogRunning_;
        unsigned watchdogMs_;
        bool watchdogAbort_;
        std::string runningName_;
#   ifdef SENF_DEBUG
        std::string runningBacktrace_;
#   endif
        unsigned watchdogCount_;
        unsigned hangCount_;
        bool yield_;

        friend void senf::scheduler::restart();
        friend class singleton<FIFORunner>;
    };

}}}

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
