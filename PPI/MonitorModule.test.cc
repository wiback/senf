// $Id$
//
// Copyright (C) 2009 
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
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
    \brief MonitorModule.test unit tests */

//#include "MonitorModule.test.hh"
//#include "MonitorModule.test.ih"

// Custom includes
#include "MonitorModule.hh"
#include "DebugModules.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    
    class PacketCounter : public senf::ppi::module::MonitorModule
    {
        SENF_PPI_MODULE(PacketCounter);
    public:
        PacketCounter() : count (0) {}
        void v_handlePacket(senf::Packet const & p) { ++count; }
        unsigned count;
    };

}

BOOST_AUTO_UNIT_TEST(monitorModulePassthrough)
{
    senf::ppi::module::debug::ActiveSource source;
    senf::ppi::module::debug::PassiveSink sink;
    PacketCounter counter;
    
    senf::ppi::connect(source, counter);
    senf::ppi::connect(counter, sink);

    senf::ppi::init();
    
    senf::Packet p (senf::DataPacket::create());

    BOOST_CHECK_EQUAL( counter.count, 0u );
    source.submit(p);
    BOOST_CHECK_EQUAL( counter.count, 1u );
    BOOST_CHECK( sink.pop_front() == p );
    BOOST_CHECK( source );
    sink.throttle();
    BOOST_CHECK( ! source );
    sink.unthrottle();
    BOOST_CHECK( source );
}

BOOST_AUTO_UNIT_TEST(monitorModuleNoPassthrough)
{
    senf::ppi::module::debug::ActiveSource source;
    PacketCounter counter;

    senf::ppi::connect(source, counter);

    senf::ppi::init();

    senf::Packet p (senf::DataPacket::create());
    
    BOOST_CHECK_EQUAL( counter.count, 0u );
    source.submit(p);
    BOOST_CHECK_EQUAL( counter.count, 1u );
    BOOST_CHECK( source );
}

BOOST_AUTO_UNIT_TEST(monitorModuleDynamicConnect)
{
    senf::ppi::module::debug::ActiveSource source;
    PacketCounter counter;

    senf::ppi::connect(source, counter);

    senf::ppi::init();

    senf::Packet p (senf::DataPacket::create());
    
    BOOST_CHECK_EQUAL( counter.count, 0u );
    source.submit(p);
    BOOST_CHECK_EQUAL( counter.count, 1u );
    BOOST_CHECK( source );

    senf::ppi::module::debug::PassiveSink sink;
    senf::ppi::connect(counter, sink);
    senf::ppi::init(); // Needs to be called since I'm not running in the scheduler !!

    BOOST_CHECK( source );
    source.submit(p);
    BOOST_CHECK_EQUAL( counter.count, 2u );
    BOOST_CHECK( sink.pop_front() == p );
    sink.throttle();
    BOOST_CHECK( ! source );
    sink.unthrottle();
    BOOST_CHECK( source );

    counter.output.disconnect();
    senf::ppi::init(); // Needs to be called since I'm not running in the scheduler !!

    BOOST_CHECK( source );
    source.submit(p);
    BOOST_CHECK_EQUAL( counter.count, 3u );
    
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
