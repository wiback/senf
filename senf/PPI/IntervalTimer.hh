// $Id$
//
// Copyright (C) 2007
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
