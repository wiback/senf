//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief MonitorModule unit tests */

#include "MonitorModule.hh"

// Custom includes
#include "DebugModules.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

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

SENF_AUTO_TEST_CASE(monitorModulePassthrough)
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

SENF_AUTO_TEST_CASE(monitorModuleNoPassthrough)
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

SENF_AUTO_TEST_CASE(monitorModuleDynamicConnect)
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
