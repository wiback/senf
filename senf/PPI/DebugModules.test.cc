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
    \brief DebubgModules unit tests */

#define SENF_LOG_CONF (( (senf)(log)(Debug), (_), VERBOSE ))

#include "DebugModules.hh"

// Custom includes
#include <algorithm>
#include <sstream>
#include <senf/Packets/Packets.hh>
#include "Setup.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace debug = senf::ppi::module::debug;
namespace ppi = senf::ppi;

SENF_AUTO_TEST_CASE(debugModules)
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

SENF_AUTO_TEST_CASE(activeFeederSource)
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

SENF_AUTO_TEST_CASE(activeFeederSink)
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

SENF_AUTO_TEST_CASE(logSink)
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
