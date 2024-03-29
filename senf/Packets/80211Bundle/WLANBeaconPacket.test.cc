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
    \brief 802.11 WLAN Beacon Packet unit tests */

#include "WLANBeaconPacket.hh"

// Custom includes
#include "WLANPacket.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(WLANBeaconPacket_parse)
{
    unsigned char data[] = {
        0x3a, 0x30, 0xaa, 0x4c, 0x9c, 0x00, 0x00, 0x00,  // timestamp
        0x64, 0x00,  // beacon interval
        0x01, 0x05,  // capability information
        0x00, 0x05, 0x62, 0x6f, 0x78, 0x43, 0x31, //SSID
        0x01, 0x08, 0x8c, 0x12, 0x98, 0x24, 0xb0, 0x48, 0x60, 0x6c,  // supported rates
        0x03, 0x01, 0x40,  // DS parameter set
        0x05, 0x04, 0x00, 0x01, 0x00, 0x00,  // TIM
        0x07, 0x2a, 0x4e, 0x41, 0x49, 0x24, 0x01, 0x11, 0x28, 0x01, 0x11,
        0x2c, 0x01, 0x11, 0x30, 0x01, 0x11, 0x34, 0x01, 0x17, 0x38, 0x01,
        0x17, 0x3c, 0x01, 0x17, 0x40, 0x01, 0x17, 0x95, 0x01, 0x1e, 0x99,
        0x01, 0x1e, 0x9d, 0x01, 0x1e, 0xa1, 0x01, 0x1e, 0xa5, 0x01, 0x1e,  // Country information
        0x20, 0x01, 0x42,  // power constraint
        0xdd, 0x18, 0x00, 0x50, 0xf2, 0x02, 0x01, 0x01, 0x88, 0x00, 0x02,
        0xa3, 0x00, 0x00, 0x27, 0xa4, 0x00, 0x00, 0x42, 0x43, 0x5e, 0x00,
        0x62, 0x32, 0x2f, 0x00,   // vendor specific
    };
    senf::WLANBeaconPacket beacon (senf::WLANBeaconPacket::create(data));

    BOOST_CHECK_EQUAL( beacon->timestamp(), 0x0000009C4CAA303AuLL);
    BOOST_CHECK_EQUAL( beacon->beaconInterval(), 100u);
    BOOST_CHECK_EQUAL( beacon->ssidIE().length(), 5);
    BOOST_CHECK_EQUAL( beacon->ssidIE().value().value(), "boxC1");
    BOOST_CHECK_EQUAL( beacon->ssid().value(), "boxC1");

    BOOST_CHECK(  beacon->ieList().contains<senf::WLANPowerConstraintInfoElementParser>() );
    BOOST_CHECK(! beacon->ieList().contains(0xff) );

    typedef senf::WLANBeaconPacket::Parser::ieList_t::container_type ieListContainer_t;
    ieListContainer_t ieListContainer (beacon->ieList());
    BOOST_CHECK_EQUAL( ieListContainer.size(), 5);

    ieListContainer_t::iterator i (ieListContainer.begin());
    BOOST_CHECK_EQUAL( i->type(), 0x03);  // DS parameter set
    ++i;
    BOOST_CHECK_EQUAL( i->type(), 0x05);  // TIM
    ++i;
    BOOST_CHECK_EQUAL( i->type(), 0x07);  // Country information
    ++i;
    //power constraint
    BOOST_CHECK_EQUAL( i->type(), senf::WLANPowerConstraintInfoElementParser::typeId+0);
    BOOST_CHECK( i->is<senf::WLANPowerConstraintInfoElementParser>());
    senf::WLANPowerConstraintInfoElementParser ie (i->as<senf::WLANPowerConstraintInfoElementParser>());
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

SENF_AUTO_TEST_CASE(WLANBeaconPacket_create)
{
    senf::WLANBeaconPacket p (senf::WLANBeaconPacket::create());
    p->timestamp() << 0x0000009C4CAA303AuLL;
    p->beaconInterval() << 100u;
    p->ssidIE().value() << "boxC1";

    typedef senf::WLANBeaconPacket::Parser::ieList_t::container_type ieListContainer_t;
    ieListContainer_t ieListContainer (p->ieList());
    senf::WLANPowerConstraintInfoElementParser ie (
            ieListContainer.push_back_space().init<senf::WLANPowerConstraintInfoElementParser>() );
    ie.value() << 0x42;

    p.finalizeThis();

    unsigned char data[] = {
        0x3a, 0x30, 0xaa, 0x4c, 0x9c, 0x00, 0x00, 0x00,  // timestamp
        0x64, 0x00,  // beacon interval
        0x00, 0x00,  // capability information
        0x00, 0x05, 0x62, 0x6f, 0x78, 0x43, 0x31, //SSID
        0x01, 0x00,  // supported rates
        0x20, 0x01, 0x42,  // power constraint
    };
    SENF_CHECK_EQUAL_COLLECTIONS( p.data().begin(), p.data().end(),
            data, data+sizeof(data) );

    senf::WLANPacket_MgtFrame mgt (senf::WLANPacket_MgtFrame::createBefore(p));
    mgt.finalizeThis();

    BOOST_CHECK_EQUAL( mgt->type(),    0u);
    BOOST_CHECK_EQUAL( mgt->subtype(), 8u);
}

SENF_AUTO_TEST_CASE(WLANBeaconPacket_parse_ht)
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

    typedef senf::WLANBeaconPacket::Parser::ieList_t::container_type ieListContainer_t;
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
    BOOST_CHECK( beacon->ieList().contains<senf::WLANHTOperationInfoElementParser>() );
    senf::WLANHTOperationInfoElementParser htOpIE (beacon->ieList().find<senf::WLANHTOperationInfoElementParser>());
    BOOST_CHECK_EQUAL( htOpIE.primaryChannel(),                36u );
    BOOST_CHECK_EQUAL( htOpIE.operationInfo().channelWidth(), true );
}

SENF_AUTO_TEST_CASE(WLANBeaconPacket_parse_mesh_vht)
{
    unsigned char data[] = {
            0x3b, 0xc0, 0xb5, 0x1b, 0x00, 0x00, 0x00, 0x00,
            0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x08,
            0x8c, 0x12, 0x98, 0x24, 0xb0, 0x48, 0x60, 0x6c,
            0x03, 0x01, 0x24, 0x05, 0x04, 0x01, 0x02, 0x00,
            0x00, 0x2d, 0x1a, 0xef, 0x19, 0x1b, 0xff, 0xff,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x3d, 0x16, 0x24,
            0x05, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x72, 0x02, 0x34,
            0x34, 0x71, 0x07, 0x01, 0x01, 0x00, 0x01, 0x00,
            0x00, 0x09, 0xbf, 0x0c, 0xb2, 0x01, 0x80, 0x33,
            0xfa, 0xff, 0x00, 0x00, 0xfa, 0xff, 0x00, 0x00,
            0xc0, 0x05, 0x01, 0x2a, 0x00, 0xff, 0xff
    };

    senf::WLANBeaconPacket beacon (senf::WLANBeaconPacket::create(data));

    std::ostringstream oss (std::ostringstream::out);
    SENF_CHECK_NO_THROW( beacon.dump(oss));

    BOOST_CHECK_EQUAL( beacon->timestamp(), 0x000000001bb5c03buLL);
    BOOST_CHECK_EQUAL( beacon->ssidIE().length(), 0);
    BOOST_CHECK_EQUAL( beacon->ssid().value(), "");

    BOOST_CHECK( beacon->ieList().contains<senf::WLANMeshIdInfoElementParser>());
    BOOST_CHECK_EQUAL( beacon->ieList().find<senf::WLANMeshIdInfoElementParser>().value(), "44");

    BOOST_CHECK( beacon->ieList().contains<senf::WLANVHTCapabilitiesInfoElementParser>());
    senf::WLANVHTCapabilitiesInfoElementParser vhtCapaParser (beacon->ieList().find<
    		senf::WLANVHTCapabilitiesInfoElementParser>());
    senf::WLANVHTCapabilitiesInfoFieldParser vhtCapaInfoParser (vhtCapaParser.capabilitiesInfo());
    BOOST_CHECK_EQUAL(vhtCapaInfoParser.maxMPDULength(),                    2u    );
    BOOST_CHECK_EQUAL(vhtCapaInfoParser.supportedChannelWidthSet(),         0u    );
    BOOST_CHECK_EQUAL(vhtCapaInfoParser.rxLDPC(),                           true  );
    BOOST_CHECK_EQUAL(vhtCapaInfoParser.shortGIfor80MHz(),                  true  );
    BOOST_CHECK_EQUAL(vhtCapaInfoParser.shortGIfor160_8080MHz(),            false );
    BOOST_CHECK_EQUAL(vhtCapaInfoParser.txSTBC(),                           true  );
    BOOST_CHECK_EQUAL(vhtCapaInfoParser.rxSTBC(),                           1u    );
    BOOST_CHECK_EQUAL(vhtCapaInfoParser.suBeamformer(),                     false );
    BOOST_CHECK_EQUAL(vhtCapaInfoParser.suBeamformee(),                     false );
    BOOST_CHECK_EQUAL(vhtCapaInfoParser.compressedSteeringNrOfBFAntennas(), 0u    );
    BOOST_CHECK_EQUAL(vhtCapaInfoParser.nrOfSoundingDimensions(),           0u    );
    BOOST_CHECK_EQUAL(vhtCapaInfoParser.muBeamformer(),                     false );
    BOOST_CHECK_EQUAL(vhtCapaInfoParser.muBeamformee(),                     false );
    BOOST_CHECK_EQUAL(vhtCapaInfoParser.vhtTxOpPowerSave(),                 false );
    BOOST_CHECK_EQUAL(vhtCapaInfoParser.htc_vhtCapable(),                   false );
    BOOST_CHECK_EQUAL(vhtCapaInfoParser.maxAMPDULengthExponent(),           7u    );
    BOOST_CHECK_EQUAL(vhtCapaInfoParser.vhtLinkAdaptationCapable(),         0u    );
    BOOST_CHECK_EQUAL(vhtCapaInfoParser.rxAntennaPatternConsistency(),      true  );
    BOOST_CHECK_EQUAL(vhtCapaInfoParser.txAntennaPatternConsistency(),      true  );

    vhtCapaInfoParser.maxAMPDULengthExponent(7);
    BOOST_CHECK_EQUAL(vhtCapaInfoParser.maxAMPDULengthExponent(),           7u    );

    senf::WLANSupportedVHTMCSSetParser supportedMCSSet (vhtCapaParser.supportedMCSSet());
    BOOST_CHECK_EQUAL(supportedMCSSet.rxVHTMCSMap().maxMCS1SS(), 2u );
    BOOST_CHECK_EQUAL(supportedMCSSet.rxVHTMCSMap().maxMCS2SS(), 2u );
    BOOST_CHECK_EQUAL(supportedMCSSet.rxVHTMCSMap().maxMCS3SS(), 3u );
    BOOST_CHECK_EQUAL(supportedMCSSet.rxVHTMCSMap().maxMCS4SS(), 3u );
    BOOST_CHECK_EQUAL(supportedMCSSet.rxVHTMCSMap().maxMCS5SS(), 3u );
    BOOST_CHECK_EQUAL(supportedMCSSet.rxVHTMCSMap().maxMCS6SS(), 3u );
    BOOST_CHECK_EQUAL(supportedMCSSet.rxVHTMCSMap().maxMCS7SS(), 3u );
    BOOST_CHECK_EQUAL(supportedMCSSet.rxVHTMCSMap().maxMCS8SS(), 3u );
    BOOST_CHECK_EQUAL(supportedMCSSet.rxHighestSupportedDataRate().value(), 0u );

    senf::WLANVHTOperationInfoElementParser vhtOperationInfoParser (beacon->ieList().find<
    		senf::WLANVHTOperationInfoElementParser>());
    BOOST_CHECK_EQUAL(vhtOperationInfoParser.operationInfo().channelWidth(),             1 );
    BOOST_CHECK_EQUAL(vhtOperationInfoParser.operationInfo().channelCenterFrequency0(), 42 );
    BOOST_CHECK_EQUAL(vhtOperationInfoParser.operationInfo().channelCenterFrequency1(),  0 );

    typedef senf::WLANBeaconPacketParser::ieList_t::container_type ieListContainer_t;
    ieListContainer_t ieListContainer (beacon->ieList());
    BOOST_CHECK_EQUAL( ieListContainer.size(), 8);
}

SENF_AUTO_TEST_CASE(WLANBeaconPacket_parse_malformed)
{
    unsigned char data[] = {
              0x08, 0x2c, 0x22, 0x07, 0x00, 0x00, 0x00, 0x00,
              0x64, 0x00, 0x02, 0x00, 0x00, 0x18, 0x57, 0x69,
              0x42, 0x41, 0x43, 0x4b, 0x2d, 0x30, 0x30, 0x3a,
              0x30, 0x30, 0x3a, 0x30, 0x30, 0x3a, 0x30, 0x30,
              0x3a, 0x35, 0x30, 0x3a, 0x38, 0x30, 0x01, 0x08,
              0x8c, 0x12, 0x98, 0x24, 0xb0, 0x48, 0x60, 0x6c,
              0x06, 0x02, 0x00, 0x00, 0x80, 0xa2, 0xd5, 0xce,
              0xce, 0x6b, 0xbc, 0xfa, 0x08, 0xaf, 0xd5, 0xce,
              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00, 0x71, 0x07, 0x00, 0x00,
              0x0a, 0x01, 0x77, 0x6c, 0x61, 0x6e, 0x63, 0x6f,
              0x6e, 0x66, 0x69, 0x67, 0xdd, 0x07, 0x00, 0x50,
              0xf2, 0x02, 0x00, 0x01, 0x00
    };

    senf::WLANBeaconPacket beacon (senf::WLANBeaconPacket::create(data));

    BOOST_CHECK_EQUAL( beacon->timestamp(), 0x0000000007222c08uLL);
    BOOST_CHECK_EQUAL( beacon->ssid().value(), "WiBACK-00:00:00:00:50:80");
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
