// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
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
    \brief Route.test unit tests */

//#include "Route.test.hh"
//#include "Route.test.ih"

// Custom includes
#include "Route.hh"
#include "DebugEvent.hh"
#include "DebugModules.hh"
#include "Module.hh"
#include "Setup.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace ppi = senf::ppi;
namespace connector = ppi::connector;
namespace module = ppi::module;
namespace debug = module::debug;

namespace {
    class RouteTester : public module::Module
    {
        SENF_PPI_MODULE(RouteTester);

    public:
        connector::ActiveInput<> activeIn;
        connector::PassiveInput<> passiveIn;

        connector::ActiveOutput<> activeOut;
        connector::PassiveOutput<> passiveOut;

        ppi::DebugEvent event;

        ppi::ForwardingRoute * rt;
        
        RouteTester() : events(0), throttles(0) {
                   route( activeIn,  activeOut  );  // non-forwarding
            rt = & route( activeIn,  passiveOut );  // forward throttling
                   route( passiveIn, activeOut  );  // backward throttling
                   route( passiveIn, passiveOut );  // non-forwarding
                   route( event,     activeOut  );  // forward event throttling
                   route( activeIn,  event      );  // backward event throttling

            passiveIn.onRequest(&RouteTester::inputRequest);
            passiveOut.onRequest(&RouteTester::outputRequest);
            registerEvent(event, &RouteTester::onEvent);

            activeIn.onThrottle(&RouteTester::throttleRequest);
            activeIn.onUnthrottle(&RouteTester::unthrottleRequest);
            activeOut.onThrottle(&RouteTester::throttleRequest);
            activeOut.onUnthrottle(&RouteTester::unthrottleRequest);
        }

        void inputRequest() {
            activeOut(passiveIn());
        }

        void outputRequest() {
            passiveOut(activeIn());
        }

        void onEvent() {
            ++ events;
        }

        void throttleRequest() {
            ++ throttles;
        }
        
        void unthrottleRequest() {
            -- throttles;
        }

        unsigned events;
        int throttles;
    };
}

BOOST_AUTO_UNIT_TEST(route)
{
    debug::PassiveSource passiveSource;
    debug::ActiveSource activeSource;
    debug::PassiveSink passiveSink;
    debug::ActiveSink activeSink;
    RouteTester tester;

    ppi::connect(passiveSource, tester.activeIn);
    ppi::connect(activeSource, tester.passiveIn);
    ppi::connect(tester.activeOut, passiveSink);
    ppi::connect(tester.passiveOut, activeSink);

    ppi::init();

    senf::Packet p1 (senf::DataPacket::create());
    senf::Packet p2 (senf::DataPacket::create());

    passiveSource.submit(p1);
    activeSource.submit(p2);

    BOOST_CHECK( p2 == passiveSink.front() );

    // The passive source is not throttled at this point since it has packets in queue

    passiveSink.input.throttle();
    BOOST_CHECK( passiveSink.input.throttled() );
    BOOST_CHECK( ! tester.activeOut );
    BOOST_CHECK_EQUAL( tester.throttles, 1 );
    BOOST_CHECK( tester.passiveIn.throttled() );
    BOOST_CHECK( ! activeSource );
    BOOST_CHECK( ! tester.event.enabled() );

    passiveSink.input.unthrottle();
    BOOST_CHECK( activeSource );
    BOOST_CHECK( tester.event.enabled() );
    
    // Now throttle the passive source by exhausting the queue
    
    BOOST_CHECK( p1 == activeSink.request() );
    BOOST_CHECK( passiveSource.output.throttled() );
    BOOST_CHECK( ! tester.activeIn );
    BOOST_CHECK_EQUAL( tester.throttles, 1 );
    BOOST_CHECK( tester.passiveOut.throttled() );
    BOOST_CHECK( ! activeSink );
    BOOST_CHECK( ! tester.event.enabled() );
    
    passiveSource.submit(p1);
    BOOST_CHECK( activeSink );
    BOOST_CHECK( tester.event.enabled() );

    // Check correct combination of multiple throttling events

    activeSink.request();
    BOOST_CHECK( ! tester.event.enabled() );
    passiveSink.input.throttle();
    BOOST_CHECK( ! tester.event.enabled() );
    passiveSource.submit(p1);
    BOOST_CHECK( ! tester.event.enabled() );
    passiveSink.input.unthrottle();
    BOOST_CHECK( tester.event.enabled() );

    tester.rt->autoThrottling(false);

    BOOST_CHECK( p1 == activeSink.request() );
    BOOST_CHECK( passiveSource.output.throttled() );
    BOOST_CHECK( activeSink );
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
