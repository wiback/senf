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
    // when changing MAX_INDEX
    BOOST_CHECK( senf::RadiotapPacket_HeaderParser::FIELD_SIZE[
                     senf::RadiotapPacket_HeaderParser::MAX_INDEX] != 0 );
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

    BOOST_CHECK_EQUAL( p.size(), senf::RadiotapPacket_HeaderParser::fixed_bytes+0 );

    SENF_CHECK_NO_THROW( p->init_tsft()             = 81059833346uLL );
    BOOST_CHECK_EQUAL( p.size(), senf::RadiotapPacket_HeaderParser::fixed_bytes +
            senf::RadiotapPacketParser::tsft_t::fixed_bytes );
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
        0x00, 0x00, 0x20, 0x00, 0x2f, 0x48, 0x00, 0x00,
        0xbe, 0xa5, 0xaf, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x10, 0x04, 0x6c, 0x09, 0xa0, 0x00, 0xb6, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x80, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0x00, 0x0b, 0x0e, 0x26, 0xab, 0xc0,
        0x00, 0x0b, 0x0e, 0x26, 0xab, 0xc0, 0xe0, 0xec,
        0x81, 0x31, 0x0d, 0x1c, 0x72, 0x0a, 0x00, 0x00,
        0x64, 0x00, 0x31, 0x04, 0x00, 0x05, 0x46, 0x4f,
        0x4b, 0x55, 0x53, 0x01, 0x08, 0x82, 0x84, 0x8b,
        0x0c, 0x12, 0x96, 0x18, 0x24, 0x03, 0x01, 0x01,
        0x05, 0x04, 0x00, 0x01, 0x00, 0x00, 0x07, 0x06,
        0x44, 0x45, 0x20, 0x01, 0x0d, 0x12, 0x0b, 0x05,
        0x01, 0x00, 0x07, 0x00, 0x00, 0x43, 0x02, 0x00,
        0x00, 0x2a, 0x01, 0x00, 0x30, 0x14, 0x01, 0x00,
        0x00, 0x0f, 0xac, 0x04, 0x01, 0x00, 0x00, 0x0f,
        0xac, 0x04, 0x01, 0x00, 0x00, 0x0f, 0xac, 0x02,
        0x00, 0x00, 0x32, 0x04, 0x30, 0x48, 0x60, 0x6c,
        0xdd, 0x05, 0x00, 0x0b, 0x0e, 0x04, 0x00, 0xdd,
        0x22, 0x00, 0x0b, 0x0e, 0x02, 0x00, 0x00, 0x00,
        0x00, 0x12, 0x0c, 0x02, 0xa1, 0x04, 0xa2, 0x0b,
        0xa3, 0x0c, 0xa5, 0x12, 0xa6, 0x16, 0xa6, 0x18,
        0xa7, 0x24, 0xa8, 0x30, 0xab, 0x48, 0xae, 0x60,
        0xb5, 0x6c, 0xb8, 0xdd, 0x2e, 0x00, 0x0b, 0x0e,
        0x03, 0x00, 0xad, 0x67, 0x20, 0xc2, 0xc2, 0x14,
        0x0e, 0x36, 0xde, 0x3f, 0xbb, 0x2e, 0x4e, 0x02,
        0x70, 0x8a, 0x5b, 0x66, 0x59, 0x3c, 0xdb, 0xbb,
        0xc9, 0x65, 0x16, 0x99, 0x16, 0x84, 0x43, 0xaa,
        0x00, 0xa2, 0x45, 0xbc, 0xbc, 0x58, 0x9b, 0xd4,
        0x3e, 0xef, 0xca, 0xdd, 0x07, 0x00, 0x50, 0xf2,
        0x02, 0x00, 0x01, 0x01, 0xdd, 0x18, 0x00, 0x50,
        0xf2, 0x02, 0x01, 0x01, 0x01, 0x00, 0x03, 0xa4,
        0x00, 0x00, 0x27, 0xa4, 0x00, 0x00, 0x42, 0x43,
        0x5e, 0x00, 0x62, 0x32, 0x2f, 0x00, 0x0e, 0x58,
        0xcd, 0xa0
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


SENF_AUTO_UNIT_TEST(RadiotapPacket_packet_ath9k_mcs)
{
    unsigned char data[] = {
            0x00, 0x00, 0x15, 0x00, 0x2a, 0x48, 0x08, 0x00,
            0x00, 0x00, 0xa8, 0x09, 0x80, 0x04, 0xe1, 0x01,
            0x00, 0x00, 0x07, 0x00, 0x07
    };
    senf::RadiotapPacket p (senf::RadiotapPacket::create(data));

    BOOST_CHECK_EQUAL( p->version(),    0u   );
    BOOST_CHECK_EQUAL( p->mcsPresent(), true );

    BOOST_CHECK_EQUAL( p->mcs().bandwidthKnown(),     true );
    BOOST_CHECK_EQUAL( p->mcs().mcsIndexKnown(),      true );
    BOOST_CHECK_EQUAL( p->mcs().guardIntervalKnown(), true );
    BOOST_CHECK_EQUAL( p->mcs().fecTypeKnown(),       false);
    BOOST_CHECK_EQUAL( p->mcs().htFormatKnown(),      false);

    BOOST_CHECK_EQUAL( p->mcs().bandwidth(),     0);
    BOOST_CHECK_EQUAL( p->mcs().guardInterval(), 0);
    BOOST_CHECK_EQUAL( p->mcs().mcsIndex(),      7);

    p.dump(std::cout);
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
