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
    \brief FIFORunner public header */

#ifndef HH_SENF_Scheduler_FIFORunner_
#define HH_SENF_Scheduler_FIFORunner_ 1

// Custom includes
#include <signal.h>
#include <boost/utility.hpp>
#include <senf/boost_intrusive/ilist.hpp>
#include <senf/boost_intrusive/ilist_hook.hpp>
#include <senf/Utils/singleton.hh>
#include "EventManager.hh"

//#include "FIFORunner.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace scheduler {

    void restart();

namespace detail {

    class FIFORunner
        : public singleton<FIFORunner>
    {
    public:
        class TaskInfo;

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
            std::string backtrace_; // only used if SENF_BACKTRACE is defined.

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
        void watchdogError();

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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
