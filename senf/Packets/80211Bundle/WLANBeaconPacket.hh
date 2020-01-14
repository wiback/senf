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
    \brief 802.11 WLAN Beacon Packet public header */

#ifndef HH_SENF_Packets_80211Bundle_WLANBeaconPacket_
#define HH_SENF_Packets_80211Bundle_WLANBeaconPacket_ 1

// Custom includes
#include "InformationElements.hh"

//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace senf {

    struct WLANBeaconPacketParser : public PacketParserBase
    {
    #   include SENF_PARSER()

        SENF_PARSER_FIELD( timestamp,      UInt64LSBParser );
        SENF_PARSER_FIELD( beaconInterval, UInt16LSBParser );
        SENF_PARSER_SKIP ( 2, 2 );  // capability flags

        SENF_PARSER_FIELD( ssidIE, WLANSSIDInfoElementParser );
        WLANSSIDInfoElementParser::value_t ssid() const { return ssidIE().value(); }

        SENF_PARSER_FIELD( supportedRatesIE, WLANSupportedRatesInfoElementParser );

        SENF_PARSER_TLV_LIST( ieList, packetSize(), WLANGenericInfoElementParser );

        SENF_PARSER_FINALIZE( WLANBeaconPacketParser );
    };


    /** \brief WLAN Beacon frame packet

        \par Packet type (typedef):
            \ref WLANBeaconPacket

        \par Fields:

        \ingroup protocolbundle_80211
     */
    struct WLANBeaconPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<WLANBeaconPacketType>
    {
        typedef PacketTypeMixin<WLANBeaconPacketType> mixin;
        typedef ConcretePacket<WLANBeaconPacketType> packet;
        typedef WLANBeaconPacketParser parser;

        using mixin::init;
        using mixin::initSize;
        using PacketTypeBase::nextPacketRange;

        static void dump(packet p, std::ostream & os);
    };

    /** \brief WLAN Beacon frame packet typedef
        \ingroup protocolbundle_80211
     */
    typedef ConcretePacket<WLANBeaconPacketType> WLANBeaconPacket;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( WLANBeaconPacket );


    
    struct InformationElementsPacketParser : public PacketParserBase
    {
    #   include SENF_PARSER()
        SENF_PARSER_TLV_LIST( ieList, packetSize(), WLANGenericInfoElementParser );
        SENF_PARSER_FINALIZE( InformationElementsPacketParser );
    };

    /** \brief WLAN Information Element packet

        \par Packet type (typedef):
            \ref InformationElementPacket

        \par Fields:

        \ingroup protocolbundle_80211
     */
    struct InformationElementsPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<InformationElementsPacketType>
    {
        typedef PacketTypeMixin<InformationElementsPacketType> mixin;
        typedef ConcretePacket<InformationElementsPacketType> packet;
        typedef WLANBeaconPacketParser parser;

        using mixin::init;
        using mixin::initSize;
        using PacketTypeBase::nextPacketRange;

        static void dump(packet p, std::ostream & os);
    };

    /** \brief WLAN Information Element packet typedef
        \ingroup protocolbundle_80211
     */
    typedef ConcretePacket<InformationElementsPacketType> InformationElementsPacket;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( InformationElementsPacket );

    
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "WLANBeaconPacket.cci"
//#include "WLANBeaconPacket.ct"
//#include "WLANBeaconPacket.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
