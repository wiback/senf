//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief IntervalTimer public header */

#ifndef HH_SENF_PPI_IntervalTimer_
#define HH_SENF_PPI_IntervalTimer_ 1

// Custom includes
#include <senf/Scheduler/ClockService.hh>
#include <senf/Scheduler/TimerEvent.hh>
#include "Events.hh"

//#include "IntervalTimer.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {

    /** \brief IntervalTimer event information

        Information passed to the IntervalTimer event handler
     */
    struct IntervalTimerEventInfo
    {
        ClockService::clock_type expected; ///< Scheduled event time
        ClockService::clock_type intervalStart; ///< Start of the current time interval
        unsigned number;                ///< Number of the current event within the interval
    };

    /** \brief High precision regularly signaled event.

        An IntervalTimer signals an event \a eventsPerInterval times each \a interval
        nanoseconds. The event counter and timer are reset, whenever the event is disabled.

        \see IntervalTimerEventInfo

        \ingroup event_group
      */
    class IntervalTimer
        : public EventImplementation<IntervalTimerEventInfo>
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        explicit IntervalTimer(ClockService::clock_type interval,
                               unsigned eventsPerInterval=1);
        IntervalTimer();

        //\}
        //-////////////////////////////////////////////////////////////////////////

        void interval(ClockService::clock_type interval, unsigned eventsPerInterval=1);
        std::pair<ClockService::clock_type, unsigned> interval() const;

    protected:

    private:
        virtual void v_enable();
        virtual void v_disable();

        void schedule();
        void cb();

        ClockService::clock_type interval_;
        unsigned eventsPerInterval_;
        IntervalTimerEventInfo info_;
        scheduler::TimerEvent timer_;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "IntervalTimer.cci"
//#include "IntervalTimer.ct"
//#include "IntervalTimer.cti"
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
