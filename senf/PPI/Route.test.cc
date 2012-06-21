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
    \brief Route unit tests */

//#include "Route.test.hh"
//#include "Route.test.ih"

// Custom includes
#include <boost/scoped_ptr.hpp>
#include <senf/Scheduler/Scheduler.hh>
#include "Route.hh"
#include "DebugEvent.hh"
#include "DebugModules.hh"
#include "Module.hh"
#include "Setup.hh"
#include "CloneSource.hh"
#include "Joins.hh"
#include "PassiveQueue.hh"
#include <senf/Utils/membind.hh>
#include <senf/Utils/senfassert.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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

SENF_AUTO_UNIT_TEST(route)
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// test connection new modules on runtime

namespace {
    void timeout() {
        senf::scheduler::terminate();
    }

    // just a helper class for the test
    struct ModuleConnector {
        module::PriorityJoin & join_;
        ModuleConnector( module::PriorityJoin & join)
            : join_( join) {};
        void connect() {
            queue.reset(new module::PassiveQueue);
            ppi::connect( *queue, join_, 0);
        }
        boost::scoped_ptr<module::PassiveQueue> queue;
    };

    class TestSink : public module::Module
    {
        SENF_PPI_MODULE(TestSink);
    public:
        connector::PassiveInput<> input;
        TestSink() {
            noroute(input);
            input.onRequest(&TestSink::request);
        }
    private:
        void request() {
            SENF_ASSERT(input(), "TestSink called without packet");
        }
    };
}

SENF_AUTO_UNIT_TEST(connect_runtime)
{
    TestSink sink;
    module::ActiveFeeder feeder;
    module::PriorityJoin join;
    module::CloneSource source1 (senf::DataPacket::create());

    ppi::connect( source1, join);
    ppi::connect( join, feeder);
    ppi::connect( feeder, sink);

    ModuleConnector moduleConnector ( join);
    senf::scheduler::TimerEvent timer (
        "connect_runtime timer",
        senf::membind(&ModuleConnector::connect, &moduleConnector),
        senf::ClockService::now() + senf::ClockService::milliseconds(250));

    senf::scheduler::TimerEvent timeoutTimer (
        "connect_runtime test timeoutTimer", &timeout,
        senf::ClockService::now() + senf::ClockService::milliseconds(500));

    senf::ppi::run();

    BOOST_CHECK( true );
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
