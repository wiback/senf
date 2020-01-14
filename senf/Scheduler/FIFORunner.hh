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
    \brief FIFORunner public header */

#ifndef HH_SENF_Scheduler_FIFORunner_
#define HH_SENF_Scheduler_FIFORunner_ 1

// Custom includes
#include <signal.h>
#include <boost/function.hpp>
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/list_hook.hpp>
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

        typedef boost::function<void(std::string const &, std::string const &, std::string const &, unsigned, unsigned)> WatchdogCallback;

    private:
        struct TaskListTag;
        typedef boost::intrusive::list_base_hook< boost::intrusive::tag<TaskListTag> > TaskListBase;
        typedef boost::intrusive::list< TaskInfo,
                                        boost::intrusive::constant_time_size<false>,
                                        boost::intrusive::base_hook<TaskListBase> > TaskList;

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

        void watchdogCallback(WatchdogCallback const & cb);
        void startWatchdog();
        void stopWatchdog();

        void watchdogCheckpoint(const char * checkpoint);

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

        NullTask queueEnd_;
        NullTask normalPriorityEnd_;
        NullTask highPriorityEnd_;

        timer_t watchdogId_;
        bool watchdogRunning_;
        unsigned watchdogMs_;
        bool watchdogAbort_;
        const char * watchdogCheckpoint_;

        TaskInfo * runningTask_;
        std::string runningName_;
        std::string runningBacktrace_;

        WatchdogCallback watchdogCallback_;
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
