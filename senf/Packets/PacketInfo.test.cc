// $Id$
//
// Copyright (C) 2013
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
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
    \brief PacketInfo.test unit tests */

//#include "PacketInfo.test.hh"
//#include "PacketInfo.test.ih"

// Custom includes
#include "Packets.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

SENF_AUTO_UNIT_TEST(packetInfo)
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