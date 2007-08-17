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
    \brief Connectors.test unit tests */

//#include "Connectors.test.hh"
//#include "Connectors.test.ih"

// Custom includes
#include "Connectors.hh"
#include "DebugModules.hh"
#include "Setup.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace ppi = senf::ppi;
namespace debug = ppi::module::debug;

// For each type of connector we use the corresponding debug module. Additionally, we always need
// the corresponding connected module since otherwise the connectors cannot be connected anywhere
// and will be unusable.

BOOST_AUTO_UNIT_TEST(connector)
{
    // It doesn't matter, which type of connectors we use here since they are all based on
    // Connector.

    debug::ActivePacketSource source;
    debug::PassivePacketSink target;

    ppi::connect(source.output,target.input);
    ppi::init();

    BOOST_CHECK_EQUAL( & source.output.module(), & source );
    BOOST_CHECK_EQUAL( & target.input.module(), & target );
    BOOST_CHECK_EQUAL( & source.output.peer(), & target.input );
    BOOST_CHECK_EQUAL( & target.input.peer(), & source.output );
}

BOOST_AUTO_UNIT_TEST(passiveConnector)
{
    debug::ActivePacketSource source;
    debug::PassivePacketSink target;

    ppi::connect(source.output,target.input);
    ppi::init();

    // onRequest is implicitly tested within the PassivePacketSink implementation which is tested
    // in DebugModules.test.cc

    target.input.throttle();
    BOOST_CHECK( target.input.throttled() );
    BOOST_CHECK( target.input.nativeThrottled() );
    
    target.input.unthrottle();
    BOOST_CHECK( ! target.input.throttled() );
    BOOST_CHECK( ! target.input.nativeThrottled() );

    BOOST_CHECK_EQUAL( & target.input.peer(), & source.output );
}

namespace {
    
    bool called = false;
    
    void handler() { called = true; }
}

BOOST_AUTO_UNIT_TEST(activeConnector)
{
    debug::ActivePacketSource source;
    debug::PassivePacketSink target;

    ppi::connect(source.output,target.input);
    ppi::init();

    source.output.onThrottle(handler);
    BOOST_CHECK( ! called );
    target.input.throttle();
    BOOST_CHECK( called );
    called = false;
    target.input.unthrottle();
    BOOST_CHECK( ! called );
    source.output.onThrottle();
    source.output.onUnthrottle(handler);
    BOOST_CHECK( ! called );
    target.input.throttle();
    BOOST_CHECK( ! called );
    target.input.unthrottle();
    BOOST_CHECK( called );
    source.output.onUnthrottle();
    called = false;
    BOOST_CHECK( ! called );
    target.input.throttle();
    target.input.unthrottle();
    BOOST_CHECK( ! called );

    BOOST_CHECK_EQUAL( & source.output.peer(), & target.input );
}

BOOST_AUTO_UNIT_TEST(inputConnector)
{
    debug::ActivePacketSource source;
    debug::PassivePacketSink target;

    ppi::connect(source.output,target.input);
    ppi::init();

    // operator() is implicitly tested within the Active/PassivePacketSink implementation which is
    // tested in DebugModules.test.cc

    // peek() is implicitly tested within the Active/PassivePacketSink implementation

    BOOST_CHECK_EQUAL ( & target.input.peer(), & source.output );
    
    BOOST_CHECK( target.input.begin() == target.input.end() );
    BOOST_CHECK_EQUAL( target.input.queueSize(), 0u );
    BOOST_CHECK( target.input.empty() );
}

BOOST_AUTO_UNIT_TEST(outputConnector)
{
    debug::ActivePacketSource source;
    debug::PassivePacketSink target;

    ppi::connect(source.output,target.input);
    ppi::init();

    // operator() is implicitly tested within the Active/PassivePacketSource implementation which is
    // tested in DebugModules.test.cc

    BOOST_CHECK_EQUAL( & source.output.peer(), & target.input );
}

namespace {

    class PassiveInputTest
        : public ppi::module::Module
    {
        SENF_PPI_MODULE(PassiveInputTest);

    public:
        ppi::connector::PassiveInput input;

        PassiveInputTest() : counter() {
            noroute(input);
            input.onRequest(&PassiveInputTest::request);
        }

        void request() {
            ++ counter;
        }

        unsigned counter;
    };
}

BOOST_AUTO_UNIT_TEST(passiveInput)
{
    debug::ActivePacketSource source;
    PassiveInputTest target;

    ppi::connect(source.output,target.input);
    ppi::init();

    BOOST_CHECK_EQUAL( & target.input.peer(), & source.output );
    
    target.input.throttle();
    senf::Packet p (senf::DataPacket::create());
    source.submit(p);
    
    BOOST_CHECK_EQUAL( target.counter, 0u );
    BOOST_CHECK( target.input );
    BOOST_CHECK_EQUAL( target.input.queueSize(), 1u );
    target.input.unthrottle();
    BOOST_CHECK( target.input );
    BOOST_CHECK_EQUAL( target.counter, 1u );
    
    BOOST_CHECK( target.input() == p );
    BOOST_CHECK( ! target.input );
    
    source.submit(p);
    
    BOOST_CHECK_EQUAL( target.counter, 2u );
    BOOST_CHECK( target.input.throttled() );
    BOOST_CHECK( target.input() == p );
    BOOST_CHECK( ! target.input.throttled() );

    target.input.qdisc(ppi::ThresholdQueueing(2,0));

    source.submit(p);
    BOOST_CHECK ( ! target.input.throttled() );
    source.submit(p);
    BOOST_CHECK( target.input.throttled() );
    target.input();
    BOOST_CHECK( target.input.throttled() );
    target.input();
    BOOST_CHECK( ! target.input.throttled() );
}

BOOST_AUTO_UNIT_TEST(passiveOutput)
{
    debug::PassivePacketSource source;
    debug::ActivePacketSink target;

    ppi::connect(source.output,target.input);
    ppi::init();

    senf::Packet p (senf::DataPacket::create());
    source.submit(p);

    BOOST_CHECK_EQUAL( & source.output.peer(), & target.input );

    BOOST_CHECK( source.output );

    source.submit(p);
    BOOST_CHECK( target.request() == p );
    
    // connect() is tested indirectly via ppi::connect
}

BOOST_AUTO_UNIT_TEST(activeInput)
{
    debug::PassivePacketSource source;
    debug::ActivePacketSink target;

    ppi::connect(source.output,target.input);
    ppi::init();

    BOOST_CHECK_EQUAL( & target.input.peer(), & source.output );

    BOOST_CHECK ( ! target.input );

    senf::Packet p (senf::DataPacket::create());
    source.submit(p);

    BOOST_CHECK( target.input );
    BOOST_CHECK( target.request() == p );

    source.submit(p);
    target.input.request();
    BOOST_CHECK_EQUAL( target.input.queueSize(), 1u );
    BOOST_CHECK( target.input );
    BOOST_CHECK( target.request() == p );
}

BOOST_AUTO_UNIT_TEST(activeOutput)
{
    debug::ActivePacketSource source;
    debug::PassivePacketSink target;

    ppi::connect(source.output,target.input);
    ppi::init();
    
    BOOST_CHECK_EQUAL( & source.output.peer(), & target.input );
    BOOST_CHECK( source.output );
    target.input.throttle();
    BOOST_CHECK( ! source.output );

    // connect() is tested indirectly via ppi::connect
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
