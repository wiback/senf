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
    \brief TimeSource non-inline non-template implementation */

#include "TimeSource.hh"
#include "TimeSource.ih"

// Custom includes
#include <senf/Scheduler/ClockService.hh>

//#include "TimeSource.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::log::TimeSource

prefix_ senf::log::TimeSource::~TimeSource()
{}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::log::SystemTimeSource

prefix_ senf::log::time_type senf::log::SystemTimeSource::operator()()
    const
{
    return senf::ClockService::in_nanoseconds( senf::ClockService::now());
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "TimeSource.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
