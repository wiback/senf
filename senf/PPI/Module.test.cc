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
    \brief Module unit tests */

//#include "Module.test.hh"
//#include "Module.test.ih"

// Custom includes
#include <boost/scoped_ptr.hpp>
#include <senf/Utils/membind.hh>
#include "DebugEvent.hh"
#include "DebugModules.hh"
#include "Setup.hh"
#include "Module.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>
#include <boost/type_traits.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace ppi = senf::ppi;
namespace connector = ppi::connector;
namespace debug = ppi::module::debug;

namespace {
    class TestModule : public ppi::module::Module
    {
        SENF_PPI_MODULE(TestModule);

    public:
        connector::ActiveOutput<> output;

        ppi::DebugEvent event;

        TestModule() {
            noroute(output);
            registerEvent(event, &TestModule::onEvent);
        }

        void onEvent() {
            output(senf::DataPacket::create());
        }

        using ppi::module::Module::time;
    };
}

SENF_AUTO_UNIT_TEST(module)
{
    // route and registerEvent are tested in Route.test.cc

    TestModule tester;
    debug::PassiveSink sink;
    ppi::connect(tester, sink);
    ppi::init();

    tester.event.trigger();
    BOOST_CHECK_EQUAL( sink.size(), 1u );
    BOOST_CHECK( senf::ClockService::now() - tester.time() < senf::ClockService::seconds(1) );
}

namespace {

    void timeout() {
        senf::scheduler::terminate();
    }

    class InitTest : public ppi::module::Module
    {
        SENF_PPI_MODULE(InitTest);
    public:
        InitTest() : init (false) {}
        void v_init() { init = true; }

        bool init;
    };

    struct MakeInit {
        boost::scoped_ptr<InitTest> tester;
        void make() {
            tester.reset(new InitTest());
        }
        void test() {
            BOOST_REQUIRE( tester );
            BOOST_CHECK( tester->init );
        }
    };

}

SENF_AUTO_UNIT_TEST(delayedInit)
{
    MakeInit maker;
    senf::scheduler::TimerEvent timer (
        "delayedInit timer",
        senf::membind(&MakeInit::make, &maker),
        senf::ClockService::now() + senf::ClockService::milliseconds(250) );
    senf::scheduler::TimerEvent testTimer (
        "delayedInit test",
        senf::membind(&MakeInit::test, &maker),
        senf::ClockService::now() + senf::ClockService::milliseconds(500) );
    senf::scheduler::TimerEvent timeoutTimer (
        "delayedInit timeout",
        &timeout,
        senf::ClockService::now() + senf::ClockService::milliseconds(750) );

    senf::ppi::run();

    BOOST_REQUIRE( maker.tester );
    BOOST_CHECK( maker.tester->init );
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
