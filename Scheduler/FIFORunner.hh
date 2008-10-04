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
            explicit TaskInfo(std::string const & name);
            virtual ~TaskInfo();

            void run();

        protected:
            void setRunnable();
            
        private:
            virtual void v_run() = 0;
            virtual bool v_enabled() const;

            bool runnable_;
#       ifdef SENF_DEBUG
            std::string backtrace_;
#       endif

            friend class FIFORunner;
        };

        using singleton<FIFORunner>::instance;
        using singleton<FIFORunner>::alive;

        void enqueue(TaskInfo * task);
        void dequeue(TaskInfo * task);
        
        void run();

        void taskTimeout(unsigned ms);
        unsigned taskTimeout() const;

        unsigned hangCount() const;

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
