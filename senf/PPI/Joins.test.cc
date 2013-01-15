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
    \brief Joins unit tests */

//#include "Joins.test.hh"
//#include "Joins.test.ih"

// Custom includes
#include "Joins.hh"
#include "DebugModules.hh"
#include "Setup.hh"
#include <senf/Packets/Packets.hh>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

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

SENF_AUTO_UNIT_TEST(passiveJoin)
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

SENF_AUTO_UNIT_TEST(priorityJoin)
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

//SENF_AUTO_UNIT_TEST(jack_passiveJoin)
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
