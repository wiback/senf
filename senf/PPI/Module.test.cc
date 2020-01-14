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
    \brief Module unit tests */

#include "Module.hh"

// Custom includes
#include <boost/scoped_ptr.hpp>
#include <senf/Utils/membind.hh>
#include <senf/Scheduler/Scheduler.hh>
#include "DebugEvent.hh"
#include "DebugModules.hh"
#include "Setup.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

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

SENF_AUTO_TEST_CASE(module)
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

SENF_AUTO_TEST_CASE(delayedInit)
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
