// $Id$
//
// Copyright (C) 2008
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
//   Christian Niephaus <cni@berlios.de>

/** \file
    \brief RadiotapPacket unit tests */

// Custom includes
#include <sstream>
#include "RadiotapPacket.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

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

SENF_AUTO_UNIT_TEST(RadiotapPacket_parsetest)
{
    unsigned char data[] = {
        '\x00', '\x00', '\x20', '\x00', '\x2f', '\x48', '\x00', '\x00',
        '\xbe', '\xa5', '\xaf', '\x00', '\x00', '\x00', '\x00', '\x00',
        '\x10', '\x04', '\x6c', '\x09', '\xa0', '\x00', '\xb6', '\x01',
        '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        '\x80', '\x00', '\x00', '\x00', '\xff', '\xff', '\xff', '\xff',
        '\xff', '\xff', '\x00', '\x0b', '\x0e', '\x26', '\xab', '\xc0',
        '\x00', '\x0b', '\x0e', '\x26', '\xab', '\xc0', '\xe0', '\xec',
        '\x81', '\x31', '\x0d', '\x1c', '\x72', '\x0a', '\x00', '\x00',
        '\x64', '\x00', '\x31', '\x04', '\x00', '\x05', '\x46', '\x4f',
        '\x4b', '\x55', '\x53', '\x01', '\x08', '\x82', '\x84', '\x8b',
        '\x0c', '\x12', '\x96', '\x18', '\x24', '\x03', '\x01', '\x01',
        '\x05', '\x04', '\x00', '\x01', '\x00', '\x00', '\x07', '\x06',
        '\x44', '\x45', '\x20', '\x01', '\x0d', '\x12', '\x0b', '\x05',
        '\x01', '\x00', '\x07', '\x00', '\x00', '\x43', '\x02', '\x00',
        '\x00', '\x2a', '\x01', '\x00', '\x30', '\x14', '\x01', '\x00',
        '\x00', '\x0f', '\xac', '\x04', '\x01', '\x00', '\x00', '\x0f',
        '\xac', '\x04', '\x01', '\x00', '\x00', '\x0f', '\xac', '\x02',
        '\x00', '\x00', '\x32', '\x04', '\x30', '\x48', '\x60', '\x6c',
        '\xdd', '\x05', '\x00', '\x0b', '\x0e', '\x04', '\x00', '\xdd',
        '\x22', '\x00', '\x0b', '\x0e', '\x02', '\x00', '\x00', '\x00',
        '\x00', '\x12', '\x0c', '\x02', '\xa1', '\x04', '\xa2', '\x0b',
        '\xa3', '\x0c', '\xa5', '\x12', '\xa6', '\x16', '\xa6', '\x18',
        '\xa7', '\x24', '\xa8', '\x30', '\xab', '\x48', '\xae', '\x60',
        '\xb5', '\x6c', '\xb8', '\xdd', '\x2e', '\x00', '\x0b', '\x0e',
        '\x03', '\x00', '\xad', '\x67', '\x20', '\xc2', '\xc2', '\x14',
        '\x0e', '\x36', '\xde', '\x3f', '\xbb', '\x2e', '\x4e', '\x02',
        '\x70', '\x8a', '\x5b', '\x66', '\x59', '\x3c', '\xdb', '\xbb',
        '\xc9', '\x65', '\x16', '\x99', '\x16', '\x84', '\x43', '\xaa',
        '\x00', '\xa2', '\x45', '\xbc', '\xbc', '\x58', '\x9b', '\xd4',
        '\x3e', '\xef', '\xca', '\xdd', '\x07', '\x00', '\x50', '\xf2',
        '\x02', '\x00', '\x01', '\x01', '\xdd', '\x18', '\x00', '\x50',
        '\xf2', '\x02', '\x01', '\x01', '\x01', '\x00', '\x03', '\xa4',
        '\x00', '\x00', '\x27', '\xa4', '\x00', '\x00', '\x42', '\x43',
        '\x5e', '\x00', '\x62', '\x32', '\x2f', '\x00', '\x0e', '\x58',
        '\xcd', '\xa0'
    };
    senf::RadiotapPacket p (senf::RadiotapPacket::create(data));

    std::stringstream ss;
    p.dump(ss);

    BOOST_CHECK_EQUAL(ss.str(),
                      "Radiotap:\n"
                      "  version                 : 0\n"
                      "  length                  : 32\n"
                      "  MAC timestamp           : 11511230\n"
                      "  flags                   : FCSatEnd \n"
                      "  rate                    : 4\n"
                      "  channel frequency       : 2412\n"
                      "  channel flags           : 2GHz CCK \n"
                      "  antenna signal (dBm)    : -74\n"
                      "  antenna                 : 1\n"
                      "  rx flags                : \n"
                      "  fcs                     : 240700832\n"
                      "802.11 MAC Management Frame:\n"
                      "  version                 : 0\n"
                      "  type                    : 0\n"
                      "  subtype                 : 8\n"
                      "  flags                   : none\n"
                      "  duration                : 0\n"
                      "  destination             : ff:ff:ff:ff:ff:ff\n"
                      "  source                  : 00:0b:0e:26:ab:c0\n"
                      "  bss id                  : 00:0b:0e:26:ab:c0\n"
                      "  sequence number         : 3790\n"
                      "  fragment number         : 0\n");
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
