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
    \brief RateFilter non-inline non-template implementation */

#include "RateFilter.hh"
//#include "RateFilter.ih"

// Custom includes

//#include "RateFilter.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::ppi::module::RateFilter::RateFilter(ClockService::clock_type interval)
    : timer_(interval)
{
    route(input, timer_);
    route(timer_, output);
    registerEvent(timer_, &RateFilter::timeout);
}

prefix_ void senf::ppi::module::RateFilter::timeout()
{
    output(input());
}

prefix_ senf::ClockService::clock_type senf::ppi::module::RateFilter::interval()
    const
{
    return timer_.interval().first;
}

prefix_ void senf::ppi::module::RateFilter::interval(ClockService::clock_type interval)
{
    timer_.interval(interval);
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "RateFilter.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
