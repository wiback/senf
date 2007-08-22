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
    \brief IntervalTimer non-inline non-template implementation */

#include "IntervalTimer.hh"
//#include "IntervalTimer.ih"

// Custom includes
#include "Scheduler/Scheduler.hh"
#include "EventManager.hh"

//#include "IntervalTimer.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::ppi::IntervalTimer

////////////////////////////////////////
// private members

prefix_ void senf::ppi::IntervalTimer::v_enable()
{
    info_.intervalStart = manager().now();
    info_.number = 0;
    schedule();
}

prefix_ void senf::ppi::IntervalTimer::v_disable()
{
    Scheduler::instance().cancelTimeout(id_);
    id_ = 0;
}

prefix_ void senf::ppi::IntervalTimer::schedule()
{
    info_.expected = info_.intervalStart + ( interval_ * (info_.number+1) ) / eventsPerInterval_;
    id_ = Scheduler::instance().timeout(info_.expected, boost::bind(&IntervalTimer::cb,this));
}

prefix_ void senf::ppi::IntervalTimer::cb()
{
    callback(info_, info_.expected);
    if (! enabled())
        return;
    ++ info_.number;
    if (info_.number >= eventsPerInterval_) {
        info_.number = 0;
        info_.intervalStart += interval_;
    }
    schedule();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "IntervalTimer.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
