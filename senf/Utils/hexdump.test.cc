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
    \brief hexdump unit tests */

#include "hexdump.hh"

// Custom includes
#include <sstream>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(hExDuMp)
{
    char data[] = { 0x18, 0x19, 0x20, 0x21, 0x7c, 0x7d, 0x7e, 0x7f };
    std::stringstream s;
    senf::hexdump(data, data+sizeof(data), s, 10);
    BOOST_CHECK_EQUAL( s.str(), "  0000  18 19 20 21 7c  7d 7e 7f        .. !| }..\n" );
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
