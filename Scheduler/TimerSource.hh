// $Id$
//
// Copyright (C) 2009 
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
    \brief TimerSource public header */

#ifndef HH_SENF_Scheduler_TimerSource_
#define HH_SENF_Scheduler_TimerSource_ 1

// Custom includes
#include <boost/utility.hpp>
#include <signal.h>
#include "ClockService.hh"
#include "FdManager.hh"

//#include "TimerSource.mpp"
///////////////////////////////hh.p////////////////////////////////////////

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

#ifdef HAVE_TIMERFD
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

///////////////////////////////hh.e////////////////////////////////////////
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
