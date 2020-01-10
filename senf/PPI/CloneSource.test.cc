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
    \brief CloneSource unit tests */

#include "CloneSource.hh"

// Custom includes
#include <senf/Packets/Packets.hh>
#include "DebugModules.hh"
#include "Setup.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(cloneSource)
{
    senf::PacketData::byte data[] = { 0xab };
    senf::Packet p (senf::DataPacket::create(data));

    senf::ppi::module::CloneSource source (p);
    senf::ppi::module::debug::ActiveSink sink;

    senf::ppi::connect(source,sink);
    senf::ppi::init();

    BOOST_CHECK( sink.request() != p );
    BOOST_CHECK( sink.request().data()[0] == p.data()[0] );
    BOOST_CHECK( sink.request().data()[0] == p.data()[0] );

    data[0] = 0xcd;
    senf::Packet p2 (senf::DataPacket::create(data));
    source.replacePacket( p2);
    BOOST_CHECK( sink.request().data()[0] == p2.data()[0] );
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
