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
    \brief PassiveQueue.test unit tests */

//#include "PassiveQueue.test.hh"
//#include "PassiveQueue.test.ih"

// Custom includes
#include "PassiveQueue.hh"
#include "DebugModules.hh"
#include "Setup.hh"
#include "../Packets/Packets.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace ppi = senf::ppi;
namespace connector = ppi::connector;
namespace module = ppi::module;
namespace debug = module::debug;

BOOST_AUTO_UNIT_TEST(passiveQueue)
{
    debug::ActiveSource source;
    module::PassiveQueue queue;
    debug::ActiveSink sink;

    ppi::connect(source,queue);
    ppi::connect(queue,sink);
    ppi::init();

    senf::Packet p (senf::DataPacket::create());
    
    BOOST_CHECK( ! sink );
    source.submit(p);
    BOOST_CHECK( sink );
    BOOST_CHECK( ! source );
    BOOST_CHECK_EQUAL( queue.input.queueSize(), 1u );
    BOOST_CHECK( sink.request() == p );
    BOOST_CHECK( source );
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
