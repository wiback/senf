// $Id$
//
// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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

#ifndef HH_IntervalTimer_
#define HH_IntervalTimer_ 1

// Custom includes
#include "Scheduler/ClockService.hh"
#include "Events.hh"

//#include "IntervalTimer.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {

    struct IntervalTimerEventInfo
    {
        ClockService::clock_type expected;
        ClockService::clock_type intervalStart;
        unsigned number;
    };

    /** \brief
      */
    class IntervalTimer
        : public EventImplementation<IntervalTimerEventInfo>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        explicit IntervalTimer(ClockService::clock_type interval, 
                               unsigned eventsPerInterval=1);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

    protected:

    private:
        virtual void v_enable();
        virtual void v_disable();

        void schedule();
        void cb();

        ClockService::clock_type interval_;
        unsigned eventsPerInterval_;
        IntervalTimerEventInfo info_;
        unsigned id_;
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
