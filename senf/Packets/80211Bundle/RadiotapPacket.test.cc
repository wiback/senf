// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Christian Niephaus <cni@berlios.de>
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
    \brief RadiotapPacket unit tests */

// Custom includes
#include <sstream>
#include "RadiotapPacket.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

///////////////////////////////cc.p////////////////////////////////////////

SENF_AUTO_UNIT_TEST(RadiotapPacket_fieldSizes)
{
    // This test only asserts, that nobody forgot to update the FIELD_SIZE table
    // when chaning MAX_INDEX
    BOOST_CHECK( senf::RadiotapPacketParser_Header::FIELD_SIZE[
                     senf::RadiotapPacketParser_Header::MAX_INDEX] != 0 );
}

SENF_AUTO_UNIT_TEST(RadiotapPacket_packet)
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
    // BOOST_CHECK_EQUAL( p->extendedBitmaskPresent(), false);
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

    /* dump */
    std::stringstream ss;
    p.dump(ss);
    BOOST_CHECK_EQUAL( ss.str(),
                       "Radiotap:\n"
                       "  version                 : 0\n"
                       "  length                  : 26\n"
                       "  MAC timestamp           : 81059833346\n"
                       "  flags                   : FCSatEnd ShortPreamble \n"
                       "  rate                    : 12\n"
                       "  channel frequency       : 5320\n"
                       "  channel flags           : OFDM 5GHz \n"
                       "  antenna signal (dBm)    : -61\n"
                       "  antenna noise (dBm)     : -96\n"
                       "  antenna                 : 2\n"
                       "  antenna signal (dB)     : 35\n"
                       "  fcs                     : 0\n" );
}

SENF_AUTO_UNIT_TEST(RadiotapPacket_create)
{
    senf::RadiotapPacket p (senf::RadiotapPacket::create());

    BOOST_CHECK_EQUAL( p.size(), senf::RadiotapPacketParser_Header::fixed_bytes+0 );

    SENF_CHECK_NO_THROW( p->init_tsft()             = 81059833346uLL );
    SENF_CHECK_NO_THROW( p->init_rate()             =          12u   );
    SENF_CHECK_NO_THROW( p->init_dbmAntennaSignal() =         -61    );
    SENF_CHECK_NO_THROW( p->init_dbmAntennaNoise()  =         -96    );
    SENF_CHECK_NO_THROW( p->init_antenna()          =           2u   );
    SENF_CHECK_NO_THROW( p->init_dbAntennaSignal()  =          35    );

    SENF_CHECK_NO_THROW( p->init_flags());
    SENF_CHECK_NO_THROW( p->flags().shortPreamble() = true);

    SENF_CHECK_NO_THROW( p->init_channelOptions());
    SENF_CHECK_NO_THROW( p->channelOptions().freq() = 5320u)
    SENF_CHECK_NO_THROW( p->channelOptions().ofdm() = true);
    SENF_CHECK_NO_THROW( p->channelOptions().flag5ghz() = true);

    SENF_CHECK_NO_THROW( p->init_fcs() );

    p.finalizeAll();

    BOOST_CHECK_EQUAL( p->length(), 26u );
    BOOST_CHECK_EQUAL( p.size(), 30u );

    std::stringstream ss;
    p.dump(ss);
    BOOST_CHECK_EQUAL( ss.str(),
                       "Radiotap:\n"
                       "  version                 : 0\n"
                       "  length                  : 26\n"
                       "  MAC timestamp           : 81059833346\n"
                       "  flags                   : FCSatEnd ShortPreamble \n"
                       "  rate                    : 12\n"
                       "  channel frequency       : 5320\n"
                       "  channel flags           : OFDM 5GHz \n"
                       "  antenna signal (dBm)    : -61\n"
                       "  antenna noise (dBm)     : -96\n"
                       "  antenna                 : 2\n"
                       "  antenna signal (dB)     : 35\n"
                       "  fcs                     : 0\n" );

    {
        unsigned char data[] = {
            /*  0 */ 0x00,                                           // version
            /*    */ 0x00,
            /*  2 */ 0x1a, 0x00,                                     // length
            /*  4 */ 0x6f, 0x18, 0x00, 0x00,                         // presentFlags
            /*  8 */ 0x02, 0xe6, 0x8a, 0xdf, 0x12, 0x00, 0x00, 0x00, // tsft
            /* 16 */ 0x12,                                           // flags
            /* 17 */ 0x0c,                                           // rate
            /* 18 */ 0xc8, 0x14,                                     // channel frequency
            /* 20 */ 0x40, 0x01,                                     // channel flags
            /* 22 */ 0xc3,                                           // dbmAntennaSignal
            /* 23 */ 0xa0,                                           // dbmAntennaNoise
            /* 24 */ 0x02,                                           // antenna
            /* 25 */ 0x23,                                           // dbAntennaSignal
            /* 26 */ 0x0, 0x0, 0x0, 0x0                              // FCS
        };

        BOOST_CHECK_EQUAL_COLLECTIONS( p.data().begin(), p.data().end(),
                                       data, data+sizeof(data)/sizeof(data[0]) );
    }

    SENF_CHECK_NO_THROW( p->disable_flags() );
    SENF_CHECK_NO_THROW( p->disable_dbmAntennaSignal() );

    p.finalizeAll();

    {
        unsigned char data[] = {
            /*  0 */ 0x00,                                           // version
            /*    */ 0x00,
            /*  2 */ 0x19, 0x00,                                     // length
            /*  4 */ 0x4d, 0x18, 0x00, 0x00,                         // presentFlags
            /*  8 */ 0x02, 0xe6, 0x8a, 0xdf, 0x12, 0x00, 0x00, 0x00, // tsft
            /* 16 */ 0x0c,                                           // rate
            /*    */ 0x00,
            /* 18 */ 0xc8, 0x14,                                     // channel frequency
            /* 20 */ 0x40, 0x01,                                     // channel flags
            /* 22 */ 0xa0,                                           // dbmAntennaNoise
            /* 23 */ 0x02,                                           // antenna
            /* 24 */ 0x23                                            // dbAntennaSignal
        };
        BOOST_CHECK_EQUAL_COLLECTIONS( p.data().begin(), p.data().end(),
                                       data, data+sizeof(data)/sizeof(data[0]) );
    }
}

SENF_AUTO_UNIT_TEST(RadiotapPacket_packet_ath9k)
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
#if 0
    // The standard defines this field as RXFlags / TXFLags, so using the
    // 'official' radiotap parser, this test fails ...
    // Why is this field parsed using MSB byteorder ??
    BOOST_CHECK_EQUAL( p->headerFcs(), 0xd51af794);
#endif
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
