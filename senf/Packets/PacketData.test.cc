// $Id$
//
// Copyright (C) 2007
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
    \brief PacketData unit tests */

//#include "PacketData.test.hh"
//#include "PacketData.test.ih"

// Custom includes
#include "Packets.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    struct VoidPacket : public senf::PacketTypeBase {};
}

SENF_AUTO_UNIT_TEST(packetData)
{
    // We cannot simply allocate a packetData instance .. we must go through PacketInterpreterBase
    // and PacketImpl.

    senf::PacketInterpreterBase::ptr pi (senf::PacketInterpreter<VoidPacket>::create());

    senf::PacketData & d (pi->data());\

    BOOST_CHECK( d.begin() == d.end() );
    BOOST_CHECK_EQUAL( d.size(), 0u );
    BOOST_CHECK( d.empty() );

    d.insert(d.begin(), 0xabu);
    BOOST_CHECK_EQUAL( d.size(), 1u );
    BOOST_CHECK_EQUAL( d[0], 0xabu );
    BOOST_CHECK( !d.empty() );

    d.insert(d.begin(), 10, 0xcdu );
    BOOST_CHECK_EQUAL( d.size(), 11u );
    BOOST_CHECK_EQUAL( d[0], 0xcdu );
    BOOST_CHECK_EQUAL( d[9], 0xcdu );
    BOOST_CHECK_EQUAL( d[10], 0xabu );

    senf::PacketData::byte data[] =
        { 0xf0u, 0xf1u, 0xf2u, 0xf3u, 0xf4u, 0xf5u, 0xf6u, 0xf7u };
    d.insert(d.begin()+5, data, data+sizeof(data)/sizeof(data[0]));
    BOOST_CHECK_EQUAL( d.size(), 19u );
    BOOST_CHECK_EQUAL( d[4], 0xcdu );
    BOOST_CHECK_EQUAL( d[5], 0xf0u );
    BOOST_CHECK_EQUAL( d[12], 0xf7u );

    d.erase(d.begin());
    BOOST_CHECK_EQUAL( d.size(), 18u );
    BOOST_CHECK_EQUAL( d[4], 0xf0u );

    d.erase(d.begin(), d.begin()+11);
    BOOST_CHECK_EQUAL( d.size(), 7u );
    BOOST_CHECK_EQUAL( d[0], 0xf7u );

    d.resize(16u, 0xefu);
    BOOST_CHECK_EQUAL( d.size(), 16u );
    BOOST_CHECK_EQUAL( d[15], 0xefu );
    BOOST_CHECK_EQUAL( d[6], 0xabu );

    d.resize(8u, 0xbcu);
    BOOST_CHECK_EQUAL( d.size(), 8u );
    BOOST_CHECK_EQUAL( d[7], 0xefu );

    d.clear();
    BOOST_CHECK_EQUAL( d.size(), 0u );
    BOOST_CHECK( d.empty() );
}

SENF_AUTO_UNIT_TEST(safePacketIterator)
{
    // We cannot simply allocate a packetData instance .. we must go through PacketInterpreterBase
    // and PacketImpl.

    senf::PacketInterpreterBase::ptr pi (senf::PacketInterpreter<VoidPacket>::create());

    senf::PacketData & d (pi->data());

    senf::safe_data_iterator i;

    BOOST_CHECK( ! i );
    i = senf::safe_data_iterator(d);
    BOOST_CHECK( i );
    i = d.begin();
    BOOST_CHECK( i == senf::safe_data_iterator(d,d.begin()) );
    BOOST_CHECK( senf::PacketData::iterator(i) == d.begin() );

    senf::PacketData::byte data[] =
        { 0xf0u, 0xf1u, 0xf2u, 0xf3u, 0xf4u, 0xf5u, 0xf6u, 0xf7u };
    d.resize(sizeof(data)/sizeof(data[0]));
    BOOST_CHECK( senf::PacketData::iterator(i) == d.begin() );
    std::copy(data,data+sizeof(data)/sizeof(data[0]),i);

    BOOST_CHECK_EQUAL( d.size(), sizeof(data)/sizeof(data[0]) );
    BOOST_CHECK_EQUAL( *(i+sizeof(data)/sizeof(data[0])-1), 0xf7u );
    BOOST_CHECK_EQUAL( std::distance(i,senf::safe_data_iterator(d,d.end())),
                       senf::PacketData::difference_type(d.size()) );
    *(++i) = 0x01u;
    BOOST_CHECK_EQUAL( d[1], 0x01u );
    *(--i) = 0x02u;
    BOOST_CHECK_EQUAL( d[0], 0x02u );
    BOOST_CHECK( &d == &i.data() );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
