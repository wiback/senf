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
    \brief IpChecksum unit tests */

#include "IpChecksum.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(ipChecksum)
{
    unsigned char data[] = { 0x45, 0x00, 0x00, 0x28, 0x49, 0x44, 0x40, 0x00,
                    0x40, 0x06, 0x00, 0x00, 0x0a, 0xc1, 0x01, 0x06,
                    0xc2, 0x9f, 0xa4, 0xc3 };

    senf::IpChecksum summer;
    summer.feed(data, data+sizeof(data));
    BOOST_CHECK_EQUAL( summer.sum(), 0x7e62u );
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
