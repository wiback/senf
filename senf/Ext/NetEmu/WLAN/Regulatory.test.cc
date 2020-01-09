// $Id$
//
// Copyright (C) 2015
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fit.fraunhofer.de/license.html
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

/** \file
    \brief Regulatory unit tests */

#include "Regulatory.hh"

// Custom includes
#include <iostream>
//#include <senf/Scheduler/TimerEvent.hh>
//#include <senf/Scheduler/Scheduler.hh>
#include <senf/Utils/String.hh>
#include <senf/Utils/algorithm.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
using namespace senf::emu;


SENF_AUTO_TEST_CASE(regulatory)
{

    std::set<senf::emu::RegulatoryDomain> cache;

    // DFS-disabled world regulatory domain
    // DFS-disabled world regulatory domain

    RegulatoryDomain reg0;
    reg0.alpha2Country = "";
    reg0.dfsRegion = RegulatoryDomain::DFSRegion::Unset;
    reg0.rules.insert(RegulatoryRule()
                                 .frequencyRange(700000, 800000)
                                 .maxBandwidth(40000)
                                 .maxEIRP(3000) );
    reg0.rules.insert(RegulatoryRule()
                                 .frequencyRange(2402000, 2494000)
                                 .maxBandwidth(40000)
                                 .maxEIRP(3000) );
    reg0.rules.insert(RegulatoryRule()
                                 .frequencyRange(3000000, 4000000)
                                 .maxBandwidth(40000)
                                 .maxEIRP(3000) );
    reg0.rules.insert(RegulatoryRule()
                                     .frequencyRange(4900000, 6100000)
                                     .maxBandwidth(40000)
                                     .maxEIRP(3000) );
    cache.insert( reg0);

    // AB DFS_ETSI (
    // (758000 778000 20000 3000 0 ())
    // (2400000 2483500 40000 2000 0 ())
    // (5210000 5350000 80000 3000 0 ())
    // (5470000 5725000 80000 3600 0 ())
    // (57240000 65880000 2160000 4000 0 (NoOutdoor)))

    RegulatoryDomain myreg1;
    myreg1.alpha2Country = "AB";
    myreg1.dfsRegion = RegulatoryDomain::DFSRegion::ETSI;
    myreg1.rules.insert(RegulatoryRule()
                                 .frequencyRange(5470000, 5725000)
                                 .maxBandwidth(40000)
                                 .maxEIRP(3600)
                                 .dfsRequired());
    myreg1.rules.insert(RegulatoryRule()
                                 .frequencyRange(758000, 778000)
                                 .maxBandwidth(20000)
                                 .maxEIRP(3000) );
    myreg1.rules.insert(RegulatoryRule()
                                 .frequencyRange(2410000, 2483500)
                                 .maxBandwidth(40000)
                                 .maxEIRP(2000) );
    myreg1.rules.insert(RegulatoryRule()
                                 .frequencyRange(5210000, 5350000)
                                 .maxBandwidth(80000)
                                 .maxEIRP(3000) );
    myreg1.rules.insert(RegulatoryRule()
                                 .frequencyRange(57240000, 65880000)
                                 .maxBandwidth(2160000)
                                 .maxEIRP(4000)
                                 .noOutdoor());





    RegulatoryDomain myreg2( myreg1);
    myreg2.alpha2Country = "AA";

    std::cout << myreg1 << std::endl;
    std::cout << myreg2 << std::endl<<std::endl;;

    BOOST_CHECK( not ( myreg1 < myreg2));
    BOOST_CHECK( not ( myreg2 < myreg1));

    BOOST_CHECK( myreg1 == myreg2);
    BOOST_CHECK( myreg2 == myreg1);

    BOOST_CHECK( cache.find(myreg1) == cache.end());

    auto res (cache.insert(myreg1));
    BOOST_CHECK( res.second);

    for( auto const & r: cache) {
        std::cout << r << std::endl;
    }

    auto it = cache.find(reg0);
    BOOST_CHECK( it != cache.end());

    it = cache.find(myreg2);
    BOOST_CHECK( it != cache.end());
    RegulatoryDomain cachedreg( *it);
    BOOST_CHECK( cachedreg == myreg1);

    BOOST_CHECK( cachedreg == myreg2);

}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
