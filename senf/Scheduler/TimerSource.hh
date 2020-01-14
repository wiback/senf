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

        virtual void timeout(ClockService::clock_type const & timeout) = 0;
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

        virtual void timeout(ClockService::clock_type const & timeout);
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
        virtual void timeout(ClockService::clock_type const & timeout);
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

        virtual void timeout(ClockService::clock_type const & timeout);
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
