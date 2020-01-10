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
    \brief SafeIterator unit tests */

#include "Packets.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    struct VoidPacket : public senf::PacketTypeBase {};
}

SENF_AUTO_TEST_CASE(safePacketParser)
{
    senf::PacketInterpreter<VoidPacket>::ptr pi (senf::PacketInterpreter<VoidPacket>::create(
            senf::PacketInterpreterBase::size_type(6u)));
    senf::SafePacketParserWrapper<senf::UInt16Parser> p;

    BOOST_CHECK( !p );

    p =  senf::UInt16Parser(pi->data().begin(),&pi->data());

    BOOST_CHECK( p );
    (*p) = 0x1234u;

    BOOST_CHECK_EQUAL( (*p), 0x1234u );
    BOOST_CHECK_EQUAL( p->data()[0], 0x12u );

    p->data().resize(1024u);
    BOOST_CHECK_EQUAL( (*p), 0x1234u );
    (*p) = 0x2345u;
    BOOST_CHECK_EQUAL( p->data()[0], 0x23u );
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
