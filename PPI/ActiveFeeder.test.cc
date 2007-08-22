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
    \brief ActiveFeeder.test unit tests */

//#include "ActiveFeeder.test.hh"
//#include "ActiveFeeder.test.ih"

// Custom includes
#include "ActiveFeeder.hh"
#include "DebugModules.hh"
#include "Setup.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace debug = senf::ppi::module::debug;
namespace ppi = senf::ppi;
namespace module = senf::ppi::module;

BOOST_AUTO_UNIT_TEST(activeFeeder)
{
    debug::PassivePacketSource source;
    debug::PassivePacketSink sink;
    module::ActiveFeeder feeder;

    ppi::connect(source,feeder);
    ppi::connect(feeder,sink);

    source.submit(senf::DataPacket::create());
    source.submit(senf::DataPacket::create());
    source.submit(senf::DataPacket::create());

    ppi::run();

    BOOST_CHECK_EQUAL( source.size(), 0u );
    BOOST_CHECK_EQUAL( sink.size(), 3u );
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
