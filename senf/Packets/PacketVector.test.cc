//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief PacketVector.test unit tests */

#include "PacketVector.hh"

// Custom includes
#include <string>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

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

SENF_AUTO_TEST_CASE(packetVector)
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
