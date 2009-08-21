// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief RateFilter unit tests */

// Custom includes
#include "RateFilter.hh"
#include "DebugModules.hh"
#include "Setup.hh"
#include <senf/Utils/membind.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////
namespace ppi = senf::ppi;
namespace module = ppi::module;
namespace debug = module::debug;

namespace {
    void timeout() {
        senf::scheduler::terminate();
    }
}

BOOST_AUTO_UNIT_TEST(rateFilter)
{
    module::RateFilter rateFilter ( senf::ClockService::milliseconds(100) );
    debug::PassiveSource source;
    debug::PassiveSink sink;
    
    ppi::connect(source, rateFilter);
    ppi::connect(rateFilter, sink);
    
    std::string data ("TEST");
    senf::Packet p (senf::DataPacket::create(data));
    for (int i=0; i<10; i++)
        source.submit(p); 
    
    senf::scheduler::TimerEvent timer (
        "rateFilter test timer", &timeout,
        senf::ClockService::now() + senf::ClockService::milliseconds(250));
    
    senf::ppi::run();

    BOOST_CHECK_EQUAL( rateFilter.interval(), senf::ClockService::milliseconds(100) );
    BOOST_CHECK_EQUAL( sink.size(), 2u );
}

namespace {
    // just a helper class for the test
    struct RateFilter_IntervalChanger {
        module::RateFilter & rateFilter_;
        RateFilter_IntervalChanger( module::RateFilter & rateFilter)
            : rateFilter_( rateFilter) {};
        void changeInterval() {
            rateFilter_.interval( senf::ClockService::milliseconds(200));
        }
    };
}

BOOST_AUTO_UNIT_TEST(rateFilter_changeInterval)
{
    module::RateFilter rateFilter ( senf::ClockService::milliseconds(100) );
    debug::PassiveSource source;
    debug::PassiveSink sink;
    
    ppi::connect(source, rateFilter);
    ppi::connect(rateFilter, sink);
    
    std::string data ("TEST");
    senf::Packet p (senf::DataPacket::create(data));
    for (int i=0; i<10; i++)
        source.submit(p); 
    
    senf::scheduler::TimerEvent timeoutTimer (
        "rateFilter test timer", &timeout,
        senf::ClockService::now() + senf::ClockService::milliseconds(675));
    
    RateFilter_IntervalChanger intervalChanger (rateFilter);
    senf::scheduler::TimerEvent timer ( "RateFilter_IntervalChanger timer", 
        senf::membind(&RateFilter_IntervalChanger::changeInterval, intervalChanger),
        senf::ClockService::now() + senf::ClockService::milliseconds(250));
    
    senf::ppi::run();

    BOOST_CHECK_EQUAL( rateFilter.interval(), senf::ClockService::milliseconds(200) );
    BOOST_CHECK_EQUAL( sink.size(), 4);
}

///////////////////////////////cc.e////////////////////////////////////////
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
