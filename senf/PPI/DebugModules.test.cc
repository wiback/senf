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
    \brief DebubgModules unit tests */

//#include "DebubgModules.test.hh"
//#include "DebubgModules.test.ih"

// Custom includes
#include <algorithm>
#include <sstream>

#define SENF_LOG_CONF (( (senf)(log)(Debug), (_), VERBOSE ))

#include <senf/Packets/Packets.hh>
#include "DebugModules.hh"
#include "Setup.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace debug = senf::ppi::module::debug;
namespace ppi = senf::ppi;

SENF_AUTO_UNIT_TEST(debugModules)
{
    {
        debug::ActiveSource source;
        debug::PassiveSink sink;

        ppi::connect(source, sink);
        ppi::init();

        senf::PacketData::byte data[] = { 0x13u, 0x24u, 0x35u };
        senf::Packet p (senf::DataPacket::create(data));

        BOOST_CHECK( ! sink.input.throttled() );

        source.submit(p);

        BOOST_CHECK( ! sink.input.throttled() );
        BOOST_CHECK_EQUAL( sink.size(), 1u );
        BOOST_CHECK( ! sink.empty() );
        BOOST_CHECK_EQUAL(
            debug::PassiveSink::size_type(std::distance(sink.begin(),sink.end())),
            sink.size() );
        BOOST_CHECK( *sink.begin() == p );
        BOOST_CHECK( sink.front() == p );

        sink.clear();

        BOOST_CHECK( ! sink.front() );
        BOOST_CHECK( sink.empty() );
    }

    {
        debug::PassiveSource source;
        debug::ActiveSink sink;

        ppi::connect(source, sink);
        ppi::init();

        senf::PacketData::byte data[] = { 0x13u, 0x24u, 0x35u };
        senf::Packet p (senf::DataPacket::create(data));

        source.submit(p);

        BOOST_CHECK_EQUAL( source.size(), 1u );
        BOOST_CHECK_EQUAL( sink.request(), p );
        BOOST_CHECK_EQUAL( source.size(), 0u );
        BOOST_CHECK( source.empty() );
    }
}

SENF_AUTO_UNIT_TEST(activeFeederSource)
{
    debug::ActiveFeederSource source;
    debug::PassiveSink sink;

    ppi::connect(source,sink);
    source.submit(senf::DataPacket::create());
    ppi::run();

    BOOST_CHECK( source.empty() );
    BOOST_CHECK_EQUAL( source.size(), 0u );
    BOOST_CHECK_EQUAL( sink.size(), 1u );
}

SENF_AUTO_UNIT_TEST(activeFeederSink)
{
    debug::PassiveSource source;
    debug::ActiveFeederSink sink;

    ppi::connect(source,sink);
    source.submit(senf::DataPacket::create());
    ppi::run();

    BOOST_CHECK( ! sink.empty() );
    BOOST_CHECK_EQUAL( sink.size(), 1u );
    BOOST_CHECK_EQUAL( debug::ActiveFeederSink::size_type(std::distance(sink.begin(), sink.end())),
                       sink.size() );
    BOOST_CHECK( sink.front().data().empty() );
    BOOST_CHECK( sink.pop_front().data().empty() );
    BOOST_CHECK( sink.empty() );
    BOOST_CHECK( source.empty() );
}

SENF_AUTO_UNIT_TEST(logSink)
{
    senf::log::StringTarget logTarget;
    logTarget.route<senf::log::Debug,senf::log::VERBOSE>();

    debug::ActiveFeederSource source;
    debug::Logger<> logger ("Prefix text");

    ppi::connect(source,logger);
    senf::PacketData::byte data[] = { 0x13u, 0x24u, 0x35u };
    source.submit( senf::DataPacket::create(data) );
    senf::ppi::run();

    BOOST_CHECK( ! logTarget.str().empty() );
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
