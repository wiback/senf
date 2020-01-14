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
    \brief PacketInfo.test unit tests */

#include "Packets.hh"

// Custom includes

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

SENF_AUTO_TEST_CASE(packetInfo)
{
    {
        senf::PacketInfo ifo;

        {
            senf::DataPacket dp (senf::DataPacket::create());
            ifo = senf::PacketInfo(dp);

            BOOST_CHECK( ifo.is_shared() );

#ifdef SENF_DEBUG
            BOOST_CHECK_EQUAL(
                senf::pool_alloc_mixin<senf::detail::PacketImpl>::allocCounter(), 1u);
#endif
        }

        BOOST_CHECK( ! ifo.usingExternalMemory() );
        BOOST_CHECK( ! ifo.is_shared() );

#ifdef SENF_DEBUG
        BOOST_CHECK_EQUAL(
            senf::pool_alloc_mixin<senf::detail::PacketImpl>::allocCounter(), 1u);
#endif
    }

#ifdef SENF_DEBUG
    BOOST_CHECK_EQUAL(
        senf::pool_alloc_mixin<senf::detail::PacketImpl>::allocCounter(), 0u);
#endif
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
