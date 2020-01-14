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
    \brief Joins unit tests */

#include "Joins.hh"

// Custom includes
#include <senf/Packets/Packets.hh>
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

    struct PassiveJoin : public module::PassiveJoin<>
    {
        using module::PassiveJoin<>::connectors;
    };

}

SENF_AUTO_TEST_CASE(passiveJoin)
{
    debug::ActiveSource source1;
    debug::ActiveSource source2;
    PassiveJoin join;
    debug::PassiveSink sink;

    ppi::connect(source1, join);
    ppi::connect(source2, join);
    ppi::connect(join, sink);
    ppi::init();

    senf::Packet p (senf::DataPacket::create());

    source1.submit(p);
    BOOST_CHECK_EQUAL( sink.size(), 1u );
    source2.submit(p);
    BOOST_CHECK_EQUAL( sink.size(), 2u );

    sink.input.throttle();
    BOOST_CHECK( ! source1 );
    BOOST_CHECK( ! source2 );

    source1.submit(p);
    source2.submit(p);
    BOOST_CHECK_EQUAL( sink.size(), 2u );
    sink.input.unthrottle();
    BOOST_CHECK_EQUAL( sink.size(), 4u );

    BOOST_CHECK_EQUAL( join.connectors().size(), 2u);
    source1.output.disconnect();
    BOOST_CHECK_EQUAL( join.connectors().size(), 1u);
    ppi::connect(source1, join);
    ppi::init();
    BOOST_CHECK_EQUAL( join.connectors().size(), 2u);
}

SENF_AUTO_TEST_CASE(priorityJoin)
{
    debug::PassiveSource source1;
    debug::PassiveSource source2;
    module::PriorityJoin join;
    debug::ActiveSink sink;

    ppi::connect(source1, join);
    ppi::connect(source2, join);
    ppi::connect(join,sink);
    ppi::init();

    BOOST_CHECK( ! sink );

    senf::Packet p1 (senf::DataPacket::create());
    senf::Packet p2 (senf::DataPacket::create());

    source1.submit(p1);
    BOOST_CHECK( sink );
    source2.submit(p2);
    BOOST_CHECK( sink );
    BOOST_CHECK( sink.request() == p1 );
    BOOST_CHECK( sink );
    BOOST_CHECK( sink.request() == p2 );
    BOOST_CHECK( ! sink );

    source1.submit(p1);
    source2.submit(p2);
    source1.submit(p1);
    BOOST_CHECK( sink.request() == p1 );
    BOOST_CHECK( sink.request() == p1 );
    BOOST_CHECK( sink.request() == p2 );
    BOOST_CHECK( ! sink );

    debug::PassiveSource source3;
    debug::PassiveSource source4;
    ppi::connect(source3, join, 0);
    ppi::connect(source4, join, -2);
    // Ordering now: source3, source1, source4, source2

    senf::Packet p3 (senf::DataPacket::create());
    senf::Packet p4 (senf::DataPacket::create());

    source4.submit(p4);
    source3.submit(p3);
    source2.submit(p2);
    source1.submit(p1);
    BOOST_CHECK( sink.request() == p3 );
    BOOST_CHECK( sink.request() == p1 );
    BOOST_CHECK( sink.request() == p4 );
    BOOST_CHECK( sink.request() == p2 );
    BOOST_CHECK( ! sink );
}

namespace {
    struct ActiveJackSource
    {
        senf::ppi::connector::ActiveOutputJack<> output;

        debug::ActiveSource source1;
        debug::ActiveSource source2;

        ActiveJackSource()
            : output (source1.output) {}

        void flip() {
            output.reset( source2.output);
        }
    };
}

//SENF_AUTO_TEST_CASE(jack_passiveJoin)
//{
//    ActiveJackSource jackSource;
//    PassiveJoin join;
//    debug::PassiveSink sink;
//
//    ppi::connect(jackSource, join);
//    ppi::connect(join, sink);
//    ppi::init();
//
//    senf::Packet p1 (senf::DataPacket::create());
//    senf::Packet p2 (senf::DataPacket::create());
//
//    jackSource.source1.submit( p1);
//    BOOST_CHECK_EQUAL( sink.pop_front(), p1);
//
//    jackSource.flip();
//
//    jackSource.source2.submit( p2);
//    BOOST_CHECK_EQUAL( sink.pop_front(), p2);
//}

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
