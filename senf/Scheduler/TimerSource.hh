// $Id$
//
// Copyright (C) 2009
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
    \brief TimerSource public header */

#ifndef HH_SENF_Scheduler_TimerSource_
#define HH_SENF_Scheduler_TimerSource_ 1

// Custom includes
#include <signal.h>
#include <boost/noncopyable.hpp>
#include "ClockService.hh"
#include "FdManager.hh"

//#include "TimerSource.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace scheduler {
namespace detail {

    class TimerSource
        : boost::noncopyable
    {
    public:
        virtual ~TimerSource();

        virtual void timeout(ClockService::clock_type timeout) = 0;
        virtual void notimeout() = 0;

        virtual void enable() = 0;
        virtual void disable() = 0;

    protected:
        TimerSource();
    };

    class POSIXTimerSource
        : public detail::FdManager::Event, public TimerSource
    {
    public:
        POSIXTimerSource();
        ~POSIXTimerSource();

        virtual void timeout(ClockService::clock_type timeout);
        virtual void notimeout();

        virtual void enable();
        virtual void disable();

    private:
        static void sigHandler(int signal, ::siginfo_t * siginfo, void *);
        virtual void signal(int events);
        void reschedule();

        bool timeoutEnabled_;
        ClockService::clock_type timeout_;
        int timerPipe_[2];
        sigset_t sigSet_;
        bool signalEnabled_;
        timer_t timerId_;
    };

    class PollTimerSource
        : public TimerSource
    {
    public:
        virtual void timeout(ClockService::clock_type timeout);
        virtual void notimeout();

        virtual void enable();
        virtual void disable();
    };

#ifdef HAVE_TIMERFD_CREATE
    class TimerFDTimerSource
        : public detail::FdManager::Event, public TimerSource
    {
    public:
        TimerFDTimerSource();
        ~TimerFDTimerSource();

        virtual void timeout(ClockService::clock_type timeout);
        virtual void notimeout();

        virtual void enable();
        virtual void disable();

        static bool haveTimerFD();

    private:
        virtual void signal(int events);
        void reschedule();

        int timerfd_;
        bool timeoutEnabled_;
        ClockService::clock_type timeout_;
    };
#endif

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "TimerSource.cci"
//#include "TimerSource.ct"
//#include "TimerSource.cti"
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
