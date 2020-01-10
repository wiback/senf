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
    \brief IntervalTimer non-inline non-template implementation */

#include "IntervalTimer.hh"
//#include "IntervalTimer.ih"

// Custom includes
#include "EventManager.hh"

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
