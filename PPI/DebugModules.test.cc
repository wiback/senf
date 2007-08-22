// $Id$
//
// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief DebubgModules.test unit tests */

//#include "DebubgModules.test.hh"
//#include "DebubgModules.test.ih"

// Custom includes
#include <algorithm>
#include "Packets/Packets.hh"
#include "DebugModules.hh"
#include "Setup.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace debug = senf::ppi::module::debug;
namespace ppi = senf::ppi;

BOOST_AUTO_UNIT_TEST(debugModules)
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

BOOST_AUTO_UNIT_TEST(activeFeederSource)
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

BOOST_AUTO_UNIT_TEST(activeFeederSink)
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
