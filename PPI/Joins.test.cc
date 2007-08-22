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
    \brief Joins.test unit tests */

//#include "Joins.test.hh"
//#include "Joins.test.ih"

// Custom includes
#include "Joins.hh"
#include "DebugModules.hh"
#include "Setup.hh"
#include "Packets/Packets.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace ppi = senf::ppi;
namespace connector = ppi::connector;
namespace module = ppi::module;
namespace debug = module::debug;

BOOST_AUTO_UNIT_TEST(passiveJoin)
{
    debug::ActiveSource source1;
    debug::ActiveSource source2;
    module::PassiveJoin join;
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
}

BOOST_AUTO_UNIT_TEST(priorityJoin)
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
