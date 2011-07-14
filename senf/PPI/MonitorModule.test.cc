// $Id$
//
// Copyright (C) 2009
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
    \brief MonitorModule unit tests */

//#include "MonitorModule.test.hh"
//#include "MonitorModule.test.ih"

// Custom includes
#include "MonitorModule.hh"
#include "DebugModules.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    class PacketCounter : public senf::ppi::module::MonitorModule<>
    {
        SENF_PPI_MODULE(PacketCounter);
    public:
        PacketCounter() : count (0) {}
        void v_handlePacket(senf::Packet const & p) { ++count; }
        unsigned count;
    };

}

SENF_AUTO_UNIT_TEST(monitorModulePassthrough)
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

SENF_AUTO_UNIT_TEST(monitorModuleNoPassthrough)
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

SENF_AUTO_UNIT_TEST(monitorModuleDynamicConnect)
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
