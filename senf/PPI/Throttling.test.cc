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
    \brief Throttling unit tests */

#include "Throttling.hh"

// Custom includes
#include <senf/Packets/Packets.hh>
#include "Module.hh"
#include "Connectors.hh"
#include "DebugModules.hh"
#include "Setup.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace ppi = senf::ppi;
namespace connector = ppi::connector;
namespace module = ppi::module;
namespace debug = module::debug;

namespace {
    class QueueTester : public module::Module
    {
        SENF_PPI_MODULE(QueueTester);
    public:
        connector::PassiveInput<> input;
        connector::ActiveOutput<> output;

        QueueTester() {
            route(input, output);
            input.throttlingDisc(ppi::ThresholdThrottling(2,1));
            input.onRequest(&QueueTester::nop);
        }

        void nop() { /* no operation */ }

        void forward() {
            if (input && output)
                output(input());
        }

    };
}

SENF_AUTO_TEST_CASE(PPI_Queueing)
{
    debug::ActiveSource source;
    QueueTester tester;
    debug::PassiveSink sink;

    ppi::connect(source, tester);
    ppi::connect(tester, sink);
    ppi::init();

    senf::Packet p (senf::DataPacket::create());
    {
        BOOST_CHECK( source );
        source.submit(p);
        BOOST_CHECK( source );
        source.submit(p);
        BOOST_CHECK( ! source );
        BOOST_CHECK_EQUAL( tester.input.queueSize(), 2u );
        tester.forward();
        BOOST_CHECK_EQUAL( tester.input.queueSize(), 1u );
        BOOST_CHECK( source );
        tester.forward();
        BOOST_CHECK_EQUAL( tester.input.queueSize(), 0u );
        BOOST_CHECK( source );
        BOOST_CHECK_EQUAL( sink.size(), 2u);
        sink.clear();
    }
    {
        tester.input.throttlingDisc(ppi::ThrottlingDiscipline::NONE);
        BOOST_CHECK( source );
        source.submit(p);
        BOOST_CHECK( source );
        source.submit(p);
        BOOST_CHECK( source );
        BOOST_CHECK_EQUAL( tester.input.queueSize(), 2u );
        tester.forward();
        tester.forward();
        BOOST_CHECK( source );
        BOOST_CHECK_EQUAL( tester.input.queueSize(), 0u );
        BOOST_CHECK_EQUAL( sink.size(), 2u);
    }
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
