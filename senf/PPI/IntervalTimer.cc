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
    \brief IntervalTimer non-inline non-template implementation */

#include "IntervalTimer.hh"
//#include "IntervalTimer.ih"

// Custom includes
#include "EventManager.hh"

//#include "IntervalTimer.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::IntervalTimer

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// private members

prefix_ void senf::ppi::IntervalTimer::v_enable()
{
    if (eventsPerInterval_ <= 0)
        return;
    info_.intervalStart = manager().now();
    info_.number = 0;
    schedule();
}

prefix_ void senf::ppi::IntervalTimer::v_disable()
{
    timer_.disable();
}

prefix_ void senf::ppi::IntervalTimer::schedule()
{
    info_.expected = info_.intervalStart + ( interval_ * (info_.number+1) ) / eventsPerInterval_;
    timer_.timeout(info_.expected);
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
