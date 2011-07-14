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
