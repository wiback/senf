// $Id$
//
// Copyright (C) 2012
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
    \brief PacketVector.test unit tests */

//#include "PacketVector.test.hh"
//#include "PacketVector.test.ih"

// Custom includes
#include "PacketVector.hh"

#include <string>
#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    template <class T, unsigned N>
    std::string str(T const (& data) [N])
    { return std::string(static_cast<char const *>(static_cast<void const *>(data)),
                         static_cast<char const *>(static_cast<void const *>(data + N))); }

    template <class T>
    std::string str(T const & container)
    { return std::string(container.begin(), container.end()); }

    template <unsigned N>
    std::string strc(char const (& data) [N])
    { return std::string(static_cast<char const *>(static_cast<void const *>(data)),
                         static_cast<char const *>(static_cast<void const *>(data + N - 1))); }
}

#ifndef SENF_PACKET_STD_CONTAINER

SENF_AUTO_UNIT_TEST(packetVector)
{
    {
        senf::PacketVector vec;
        BOOST_CHECK_EQUAL( vec.size(), 0u );
        BOOST_CHECK( vec.begin() == vec.end() );

        vec.insert(vec.begin(), 0, 'a');
        BOOST_CHECK_EQUAL( vec.size(), 0u );
        BOOST_CHECK( vec.begin() == vec.end() );

        vec.insert(vec.begin(), 'a');
        BOOST_CHECK_EQUAL( vec.size(), 1u );
        BOOST_CHECK_EQUAL( vec.begin()[0], 'a' );
        BOOST_CHECK_EQUAL( vec.begin() + 1, vec.end() );

        vec.insert(vec.begin() + 1, 10, 'b');
        BOOST_CHECK_EQUAL( vec.size(), 11u );
        BOOST_CHECK_EQUAL( vec.begin()[0], 'a' );
        BOOST_CHECK_EQUAL( vec.begin()[1], 'b' );
        BOOST_CHECK_EQUAL( vec.begin()[10], 'b' );

        char const data[] = "xyz";
        vec.insert(vec.begin() + 2, data, data + sizeof(data) - 1);
        BOOST_CHECK_EQUAL( vec.size(), 14u );
        BOOST_CHECK_EQUAL( vec.begin()[0], 'a' );
        BOOST_CHECK_EQUAL( vec.begin()[1], 'b' );
        BOOST_CHECK_EQUAL( vec.begin()[2], 'x' );
        BOOST_CHECK_EQUAL( vec.begin()[3], 'y' );
        BOOST_CHECK_EQUAL( vec.begin()[4], 'z' );
        BOOST_CHECK_EQUAL( vec.begin()[5], 'b' );
        BOOST_CHECK_EQUAL( vec.begin()[13], 'b' );

        vec.erase(vec.begin() + 2);
        BOOST_CHECK_EQUAL( vec.size(), 13u );
        BOOST_CHECK_EQUAL( vec.begin()[0], 'a' );
        BOOST_CHECK_EQUAL( vec.begin()[1], 'b' );
        BOOST_CHECK_EQUAL( vec.begin()[2], 'y' );
        BOOST_CHECK_EQUAL( vec.begin()[3], 'z' );
        BOOST_CHECK_EQUAL( vec.begin()[4], 'b' );
        BOOST_CHECK_EQUAL( vec.begin()[12], 'b' );

        vec.erase(vec.begin(), vec.begin() + 3);
        BOOST_CHECK_EQUAL( vec.size(), 10u );
        BOOST_CHECK_EQUAL( vec.begin()[0], 'z' );
        BOOST_CHECK_EQUAL( vec.begin()[1], 'b' );
        BOOST_CHECK_EQUAL( vec.begin()[9], 'b' );

        vec.clear();
        BOOST_CHECK_EQUAL( vec.size(), 0u );
        BOOST_CHECK( vec.begin() == vec.end() );

        vec.insert(vec.begin(), 2040, 'c');
        vec.insert(vec.begin()+2, 40, 'd');
        BOOST_CHECK_EQUAL( vec.size(), 2080 );
        BOOST_CHECK_EQUAL( vec.begin()[0], 'c' );
        BOOST_CHECK_EQUAL( vec.begin()[1], 'c' );
        BOOST_CHECK_EQUAL( vec.begin()[2], 'd' );
        BOOST_CHECK_EQUAL( vec.begin()[41], 'd' );
        BOOST_CHECK_EQUAL( vec.begin()[42], 'c' );
        BOOST_CHECK_EQUAL( vec.begin()[2079], 'c' );
    }

    {
        senf::PacketVector::value_type storage[] = "\0\0\0TEST\0\0";
        senf::PacketVector vec (storage, 4u, 10u, 3u);
        BOOST_CHECK_EQUAL( str(vec), strc("TEST") );

        * vec.begin() = 'F';
        BOOST_CHECK_EQUAL( str(vec), strc("FEST") );
        BOOST_CHECK_EQUAL( str(storage), strc("\0\0\0FEST\0\0\0") );

        char const data[] = "DAS";
        vec.insert(vec.begin(), data, data + sizeof(data) - 1);
        BOOST_CHECK_EQUAL( str(vec), strc("DASFEST") );
        BOOST_CHECK_EQUAL( str(storage) , strc("DASFEST\0\0\0") );

        vec.insert(vec.begin() + 3, ' ');
        BOOST_CHECK_EQUAL( str(vec), strc("DAS FEST") );
        BOOST_CHECK_EQUAL( str(storage), strc("DAS FEST\0\0") );

        vec.insert(vec.end(), 2, '!');
        BOOST_CHECK_EQUAL( str(vec), strc("DAS FEST!!") );
        BOOST_CHECK_EQUAL( str(storage), strc("DAS FEST!!") );

        vec.insert(vec.end()-2, 2, ' ');
        BOOST_CHECK_EQUAL( str(vec), strc("DAS FEST  !!") );
        BOOST_CHECK_EQUAL( str(storage), strc("DAS FEST!!") );
    }
}

#endif

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
