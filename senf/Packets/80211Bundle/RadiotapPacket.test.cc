// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Christian Niephaus <cni@berlios.de>
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
    \brief RadiotapPacket unit tests */

// Custom includes
#include "RadiotapPacket.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

///////////////////////////////cc.p////////////////////////////////////////
BOOST_AUTO_UNIT_TEST(RadiotapPacket_packet)
{
    /* used madwifi 0.9.4 */
    unsigned char data[] = {
            0x00 ,0x00 ,0x1a ,0x00, 0x6f, 0x18, 0x00, 0x00,
            0x02, 0xe6, 0x8a, 0xdf, 0x12, 0x00, 0x00, 0x00,
            0x12, 0x0c, 0xc8, 0x14, 0x40, 0x01, 0xc3, 0xa0,
            0x02, 0x23, 
            0x00, 0x00, 0x00, 0x00,
    };
    senf::RadiotapPacket p (senf::RadiotapPacket::create(data));

    /* mandatory fields*/
    BOOST_CHECK_EQUAL( p->version(), 0u);
    BOOST_CHECK_EQUAL( p->length(), 26u);

    /* present flags */
    BOOST_CHECK_EQUAL( p->tsftPresent(), true);
    BOOST_CHECK_EQUAL( p->flagsPresent(), true);
    BOOST_CHECK_EQUAL( p->extendedBitmaskPresent(), false);
    BOOST_CHECK_EQUAL( p->ratePresent(), true);
    BOOST_CHECK_EQUAL( p->channelOptionsPresent(), true);
    BOOST_CHECK_EQUAL( p->fhssPresent(), false);
    BOOST_CHECK_EQUAL( p->dbmAntennaSignalPresent(), true);
    BOOST_CHECK_EQUAL( p->dbmAntennaNoisePresent(), true);
    BOOST_CHECK_EQUAL( p->lockQualityPresent(), false);
    BOOST_CHECK_EQUAL( p->txAttenuationPresent(), false);
    BOOST_CHECK_EQUAL( p->dbmTxAttenuationPresent(), false);
    BOOST_CHECK_EQUAL( p->dbTxAttenuationPresent(), false);
    BOOST_CHECK_EQUAL( p->antennaPresent(), true);
    BOOST_CHECK_EQUAL( p->dbAntennaSignalPresent(), true);
    BOOST_CHECK_EQUAL( p->dbAntennaNoisePresent(), false);

    /* data flieds */
    BOOST_CHECK_EQUAL( p->tsft(), 81059833346uLL);
    BOOST_CHECK_EQUAL( p->rate(), 12u);
    BOOST_CHECK_EQUAL( p->channelOptions().freq(), 5320u);
    BOOST_CHECK_EQUAL( p->dbmAntennaSignal(), -61);
    BOOST_CHECK_EQUAL( p->dbmAntennaNoise(), -96);
    BOOST_CHECK_EQUAL( p->antenna(), 2u);
    BOOST_CHECK_EQUAL( p->dbAntennaSignal(), 35u);

    /* flags */
    BOOST_CHECK_EQUAL( p->flags().cfp(), false);
    BOOST_CHECK_EQUAL( p->flags().shortPreamble(), true);
    BOOST_CHECK_EQUAL( p->flags().wep(), false);
    BOOST_CHECK_EQUAL( p->flags().fragmentation(), false);
    BOOST_CHECK_EQUAL( p->flags().fcsAtEnd(), true);
    BOOST_CHECK_EQUAL( p->flags().padding(), false);
    BOOST_CHECK_EQUAL( p->flags().badFCS(), false);
    BOOST_CHECK_EQUAL( p->flags().shortGI(), false);

    /* channel flags */
    BOOST_CHECK_EQUAL( p->channelOptions().ofdm(), true);
    BOOST_CHECK_EQUAL( p->channelOptions().turbo(), false);
    BOOST_CHECK_EQUAL( p->channelOptions().cck(), false);
    BOOST_CHECK_EQUAL( p->channelOptions().flag5ghz(), true);
    BOOST_CHECK_EQUAL( p->channelOptions().passive(), false);
    BOOST_CHECK_EQUAL( p->channelOptions().cckOfdm(), false);
    BOOST_CHECK_EQUAL( p->channelOptions().gfsk(), false);
    BOOST_CHECK_EQUAL( p->channelOptions().gsm(), false);
    BOOST_CHECK_EQUAL( p->channelOptions().staticTurbo(), false);
    BOOST_CHECK_EQUAL( p->channelOptions().halfRateChannel(), false);
    BOOST_CHECK_EQUAL( p->channelOptions().quarterRateChannel(), false);

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( p.dump( oss));
}

BOOST_AUTO_UNIT_TEST(RadiotapPacket_create)
{
    unsigned char data[] = {
            0x00 ,0x00 ,0x1a ,0x00, 0x6f, 0x18, 0x00, 0x00,
            0x02, 0xe6, 0x8a, 0xdf, 0x12, 0x00, 0x00, 0x00,
            0x02, 0x0c, 0xc8, 0x14, 0x40, 0x01, 0xc3, 0xa0,
            0x02, 0x23
    };

    senf::RadiotapPacket p (senf::RadiotapPacket::create());

    SENF_CHECK_NO_THROW( p->init_tsft());
    SENF_CHECK_NO_THROW( p->tsft()=81059833346uLL);

    SENF_CHECK_NO_THROW( p->init_rate());
    SENF_CHECK_NO_THROW( p->rate() = 12u);
    SENF_CHECK_NO_THROW( p->init_dbmAntennaSignal());
    SENF_CHECK_NO_THROW( p->dbmAntennaSignal() = -61);
    SENF_CHECK_NO_THROW( p->init_dbmAntennaNoise());
    SENF_CHECK_NO_THROW( p->dbmAntennaNoise() = -96);
    SENF_CHECK_NO_THROW( p->init_antenna());
    SENF_CHECK_NO_THROW( p->antenna() = 2u);
    SENF_CHECK_NO_THROW( p->init_dbAntennaSignal());
    SENF_CHECK_NO_THROW( p->dbAntennaSignal() = 35);

    SENF_CHECK_NO_THROW( p->init_flags());
    SENF_CHECK_NO_THROW( p->flags().cfp() = false);
    SENF_CHECK_NO_THROW( p->flags().shortPreamble() = true);
    SENF_CHECK_NO_THROW( p->flags().wep() = false);
    SENF_CHECK_NO_THROW( p->flags().fragmentation() = false);
    // SENF_CHECK_NO_THROW( p->flags().fcsAtEnd() = true);
    SENF_CHECK_NO_THROW( p->flags().padding() = false);
    SENF_CHECK_NO_THROW( p->flags().badFCS() = false);
    SENF_CHECK_NO_THROW( p->flags().shortGI() = false);

    SENF_CHECK_NO_THROW( p->init_channelOptions());
    SENF_CHECK_NO_THROW( p->channelOptions().freq() = 5320u)
    SENF_CHECK_NO_THROW( p->channelOptions().ofdm() = true);
    SENF_CHECK_NO_THROW( p->channelOptions().turbo() = false);
    SENF_CHECK_NO_THROW( p->channelOptions().cck() = false);
    SENF_CHECK_NO_THROW( p->channelOptions().flag5ghz() = true);
    SENF_CHECK_NO_THROW( p->channelOptions().passive() = false);
    SENF_CHECK_NO_THROW( p->channelOptions().cckOfdm() = false);
    SENF_CHECK_NO_THROW( p->channelOptions().gfsk() = false);
    SENF_CHECK_NO_THROW( p->channelOptions().gsm() = false);
    SENF_CHECK_NO_THROW( p->channelOptions().staticTurbo() = false);
    SENF_CHECK_NO_THROW( p->channelOptions().halfRateChannel() = false);
    SENF_CHECK_NO_THROW( p->channelOptions().quarterRateChannel() = false);

    p.finalizeAll();

    BOOST_CHECK_EQUAL( p->length(), 26u );
    BOOST_CHECK_EQUAL_COLLECTIONS( p.data().begin(), p.data().end(),
                                   data, data+sizeof(data)/sizeof(data[0]) );
}

BOOST_AUTO_UNIT_TEST(RadiotapPacket_packet_ath9k)
{
    /* radiotap packet from ath9k with atheros card*/
    unsigned char data[] = {
            0x00, 0x00, 0x20, 0x00, 0x6f, 0x48, 0x00, 0x00, 
            0x87, 0xbb, 0x91, 0x7c, 0x3b, 0x00, 0x00, 0x00,
            0x00, 0x04, 0x85, 0x09, 0x80, 0x04, 0xb2, 0xa1,
            0x00, 0x00, 0x00, 0x00, 0xd5, 0x1a, 0xf7, 0x94,
            0x00, 0x00, 0x00, 0x00,
    };
    senf::RadiotapPacket p (senf::RadiotapPacket::create(data));

    /* mandatory fields*/
    BOOST_CHECK_EQUAL( p->version(), 0u);
    BOOST_CHECK_EQUAL( p->length(), 32u);

    BOOST_CHECK_EQUAL( p->dbmAntennaSignal(), -78);
    BOOST_CHECK_EQUAL( p->dbmAntennaNoise(), -95);
    BOOST_CHECK_EQUAL( p->headerFcs(), 0xd51af794);
    BOOST_CHECK_EQUAL( p->antenna(), 0u);
}



// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
