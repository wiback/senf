// $Id$
//
// Copyright (C) 2007
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
    \brief IntervalTimer public header */

#ifndef HH_SENF_PPI_IntervalTimer_
#define HH_SENF_PPI_IntervalTimer_ 1

// Custom includes
#include <senf/Scheduler/ClockService.hh>
#include "Events.hh"

//#include "IntervalTimer.mpp"
///////////////////////////////hh.p////////////////////////////////////////

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
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        explicit IntervalTimer(ClockService::clock_type interval,
                               unsigned eventsPerInterval=1);
        IntervalTimer();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

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

///////////////////////////////hh.e////////////////////////////////////////
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
