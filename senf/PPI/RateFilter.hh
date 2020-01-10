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
    \brief RateFilter public header */

#ifndef HH_SENF_PPI_RateFilter_
#define HH_SENF_PPI_RateFilter_ 1

// Custom includes
#include "Connectors.hh"
#include "Module.hh"
#include "IntervalTimer.hh"

//#include "RateFilter.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {
namespace module {

class RateFilter
    : public Module
{
    SENF_PPI_MODULE(RateFilter);

public:
    RateFilter(ClockService::clock_type interval);
    void interval(ClockService::clock_type interval);
    ClockService::clock_type interval() const;

    connector::ActiveInput<> input;
    connector::ActiveOutput<> output;

private:
    void timeout();
    ppi::IntervalTimer timer_;
};

}}} //namespaces

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "RateFilter.cci"
//#include "RateFilter.ct"
//#include "RateFilter.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
