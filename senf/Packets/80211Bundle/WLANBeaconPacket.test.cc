// $Id$
//
// Copyright (C) 2009
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
//   Thorsten Horstmann <tho@berlios.de>
//   Christian Niephaus <cni@berlios.de>

/** \file
    \brief 802.11 WLAN Beacon Packet unit tests */

// Custom includes
#include "WLANBeaconPacket.hh"
#include "WLANPacket.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_UNIT_TEST(WLANBeaconPacket_parse)
{
    unsigned char data[] = {
        0x3a, 0x30, 0xaa, 0x4c, 0x9c, 0x00, 0x00, 0x00, //timestamp
        0x64, 0x00, //beacon interval
        0x01, 0x05, //capability information
        0x00, 0x05, 0x62, 0x6f, 0x78, 0x43, 0x31, //SSID
        0x01, 0x08, 0x8c, 0x12, 0x98, 0x24, 0xb0, 0x48, 0x60, 0x6c, //supported rates
        0x03, 0x01, 0x40, //DS parameter set
        0x05, 0x04, 0x00, 0x01, 0x00, 0x00, //TIM
        0x07, 0x2a, 0x4e, 0x41, 0x49, 0x24, 0x01, 0x11, 0x28, 0x01, 0x11,
        0x2c, 0x01, 0x11, 0x30, 0x01, 0x11, 0x34, 0x01, 0x17, 0x38, 0x01,
        0x17, 0x3c, 0x01, 0x17, 0x40, 0x01, 0x17, 0x95, 0x01, 0x1e, 0x99,
        0x01, 0x1e, 0x9d, 0x01, 0x1e, 0xa1, 0x01, 0x1e, 0xa5, 0x01, 0x1e, //Country information
        0x20, 0x01, 0x42, //power constraint
        0xdd, 0x18, 0x00, 0x50, 0xf2, 0x02, 0x01, 0x01, 0x88, 0x00, 0x02,
        0xa3, 0x00, 0x00, 0x27, 0xa4, 0x00, 0x00, 0x42, 0x43, 0x5e, 0x00,
        0x62, 0x32, 0x2f, 0x00,  //vendor specific
    };
    senf::WLANBeaconPacket beacon (senf::WLANBeaconPacket::create(data));

    BOOST_CHECK_EQUAL( beacon->timestamp(), 0x0000009C4CAA303AuLL);
    BOOST_CHECK_EQUAL( beacon->beaconInterval(), 100u);
    BOOST_CHECK_EQUAL( beacon->ssidIE().length(), 5);
    BOOST_CHECK_EQUAL( beacon->ssidIE().value().value(), "boxC1");
    BOOST_CHECK_EQUAL( beacon->ssid().value(), "boxC1");

    BOOST_CHECK(  beacon->hasIE(senf::WLANPowerConstraintInfoElementParser::typeId+0) );
    BOOST_CHECK(! beacon->hasIE(0xff) );

    typedef senf::WLANBeaconPacket::Parser::ieList_t::container ieListContainer_t;
    ieListContainer_t ieListContainer (beacon->ieList());
    BOOST_CHECK_EQUAL( ieListContainer.size(), 5);

    ieListContainer_t::iterator i (ieListContainer.begin());
    BOOST_CHECK_EQUAL( i->type(), 0x03); //DS parameter set
    ++i;
    BOOST_CHECK_EQUAL( i->type(), 0x05); //TIM
    ++i;
    BOOST_CHECK_EQUAL( i->type(), 0x07); //Country information
    ++i;
    //power constraint
    BOOST_CHECK_EQUAL( i->type(), senf::WLANPowerConstraintInfoElementParser::typeId+0);
    BOOST_CHECK( i->is<senf::WLANPowerConstraintInfoElementParser>());
    senf::WLANPowerConstraintInfoElementParser ie ( i->as<senf::WLANPowerConstraintInfoElementParser>());
    BOOST_CHECK_EQUAL( ie.value(), 0x42);
    ++i;
    BOOST_CHECK_EQUAL( i->type(), 0xdd); //vendor specific
    BOOST_CHECK_EQUAL( i->length(), 0x18);
    BOOST_CHECK_EQUAL( boost::size(i->value()), 0x18);

    unsigned char value[] = {
            0x00, 0x50, 0xf2, 0x02, 0x01, 0x01, 0x88, 0x00,
            0x02, 0xa3, 0x00, 0x00, 0x27, 0xa4, 0x00, 0x00,
            0x42, 0x43, 0x5e, 0x00, 0x62, 0x32, 0x2f, 0x00
    };
    SENF_CHECK_EQUAL_COLLECTIONS( value, value+sizeof(value),
            boost::begin(i->value()), boost::end(i->value()) );

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( beacon.dump( oss ));
}

SENF_AUTO_UNIT_TEST(WLANBeaconPacket_create)
{
    senf::WLANBeaconPacket p (senf::WLANBeaconPacket::create());
    p->timestamp() << 0x0000009C4CAA303AuLL;
    p->beaconInterval() << 100u;
    p->ssidIE().value() << "boxC1";

    typedef senf::WLANBeaconPacket::Parser::ieList_t::container ieListContainer_t;
    ieListContainer_t ieListContainer (p->ieList());
    senf::WLANPowerConstraintInfoElementParser ie (
            ieListContainer.push_back_space().init<senf::WLANPowerConstraintInfoElementParser>() );
    ie.value() << 0x42;

    p.finalizeThis();

    unsigned char data[] = {
        0x3a, 0x30, 0xaa, 0x4c, 0x9c, 0x00, 0x00, 0x00, //timestamp
        0x64, 0x00, //beacon interval
        0x00, 0x00, //capability information
        0x00, 0x05, 0x62, 0x6f, 0x78, 0x43, 0x31, //SSID
        0x01, 0x00, //supported rates
        0x20, 0x01, 0x42, //power constraint
    };
    SENF_CHECK_EQUAL_COLLECTIONS( p.data().begin(), p.data().end(),
            data, data+sizeof(data) );

    senf::WLANPacket_MgtFrame mgt (senf::WLANPacket_MgtFrame::createBefore(p));
    mgt.finalizeThis();

    BOOST_CHECK_EQUAL( mgt->type(),    0u);
    BOOST_CHECK_EQUAL( mgt->subtype(), 8u);
}

SENF_AUTO_UNIT_TEST(WLANBeaconPacket_parse_ht)
{
    unsigned char data[] = {
            0x62, 0xb0, 0x82, 0x04, 0x00, 0x00, 0x00, 0x00,
            0x64, 0x00, 0x02, 0x00, 0x00, 0x04, 0x74, 0x65,
            0x73, 0x74, 0x01, 0x08, 0x8c, 0x12, 0x98, 0x24,
            0xb0, 0x48, 0x60, 0x6c, 0x06, 0x02, 0x00, 0x00,
            0x2d, 0x1a, 0xce, 0x11, 0x1b, 0xff, 0xff, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x3d, 0x16, 0x24, 0x05,
            0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0xdd, 0x07, 0x00, 0x50,
            0xf2, 0x02, 0x00, 0x01, 0x00
    };

    senf::WLANBeaconPacket beacon (senf::WLANBeaconPacket::create(data));

    BOOST_CHECK_EQUAL( beacon->timestamp(), 0x000000000482b062uLL);
    BOOST_CHECK_EQUAL( beacon->ssid().value(), "test");

    typedef senf::WLANBeaconPacket::Parser::ieList_t::container ieListContainer_t;
    ieListContainer_t ieListContainer (beacon->ieList());
    BOOST_CHECK_EQUAL( ieListContainer.size(), 4);

    ieListContainer_t::iterator i (boost::next(ieListContainer.begin(), 1));
    BOOST_CHECK_EQUAL( i->type(), senf::WLANHTCapabilitiesInfoElementParser::typeId+0);
    BOOST_CHECK_EQUAL( i->length(), 26u);
    BOOST_CHECK( i->is<senf::WLANHTCapabilitiesInfoElementParser>());
    senf::WLANHTCapabilitiesInfoElementParser htCapaIE (i->as<senf::WLANHTCapabilitiesInfoElementParser>());
    // HT Capabilities Info Field
    BOOST_CHECK_EQUAL( htCapaIE.capabilitiesInfo().ldpcCodingCapability(),      false );
    BOOST_CHECK_EQUAL( htCapaIE.capabilitiesInfo().supportedChannelWidth(),     true  );
    BOOST_CHECK_EQUAL( htCapaIE.capabilitiesInfo().smPowerSave(),               3u    );
    BOOST_CHECK_EQUAL( htCapaIE.capabilitiesInfo().htGreenfield(),              false );
    BOOST_CHECK_EQUAL( htCapaIE.capabilitiesInfo().shortGIfor20MHz(),           false );
    BOOST_CHECK_EQUAL( htCapaIE.capabilitiesInfo().shortGIfor40MHz(),           true  );
    BOOST_CHECK_EQUAL( htCapaIE.capabilitiesInfo().txSTBC(),                    true  );
    BOOST_CHECK_EQUAL( htCapaIE.capabilitiesInfo().rxSTBC(),                    1u    );
    BOOST_CHECK_EQUAL( htCapaIE.capabilitiesInfo().delayedBlockAck(),           false );
    BOOST_CHECK_EQUAL( htCapaIE.capabilitiesInfo().maxAMSDULength(),            false );
    BOOST_CHECK_EQUAL( htCapaIE.capabilitiesInfo().dsss_cckModeIn40MHz(),       true  );
    BOOST_CHECK_EQUAL( htCapaIE.capabilitiesInfo().fortyMHzIntolerant(),        false );
    BOOST_CHECK_EQUAL( htCapaIE.capabilitiesInfo().lSIGTXOPProtectionSupport(), false );
    // A-MPDU Parameters
    BOOST_CHECK_EQUAL( htCapaIE.aMPDUParameters().maxAMPDULength(),             3u    );
    BOOST_CHECK_EQUAL( htCapaIE.aMPDUParameters().minMPDUStartSpacing(),        6u    );
    // Supported MCS Set
    BOOST_CHECK_EQUAL( htCapaIE.supportedMCSSet().rxMCSBitmask()[0],            true  );
    BOOST_CHECK_EQUAL( htCapaIE.supportedMCSSet().rxMCSBitmask()[15],           true  );
    BOOST_CHECK_EQUAL( htCapaIE.supportedMCSSet().rxMCSBitmask()[16],           false );
    BOOST_CHECK_EQUAL( htCapaIE.supportedMCSSet().rxMCSBitmask()[76],           false );
    BOOST_CHECK_EQUAL( htCapaIE.supportedMCSSet().rxHighestSupoortedDataRate(), 0x0u  );
    BOOST_CHECK_EQUAL( htCapaIE.supportedMCSSet().txMCSSetDefined(),            true  );
    BOOST_CHECK_EQUAL( htCapaIE.supportedMCSSet().txRxMCSSetNotEqual(),         false );
    // HT Extended Capabilities
    // Transmit Beamforming Capabilities
    // Antenna Selection Capability
    // ...
    ++i;
    BOOST_CHECK_EQUAL( i->type(), senf::WLANHTOperationInfoElementParser::typeId+0);
    BOOST_CHECK_EQUAL( i->length(), 22u);
    BOOST_CHECK( beacon->hasIE(senf::WLANHTOperationInfoElementParser::typeId+0) );
    senf::WLANHTOperationInfoElementParser htOpIE (beacon->findIE<senf::WLANHTOperationInfoElementParser>());
    BOOST_CHECK_EQUAL( htOpIE.primaryChannel(),                36u );
    BOOST_CHECK_EQUAL( htOpIE.operationInfo().channelWidth(), true );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
