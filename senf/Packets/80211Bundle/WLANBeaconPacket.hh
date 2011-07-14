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

        SENF_PARSER_FIELD( timestamp,      UInt64LSBParser   );
        SENF_PARSER_FIELD( beaconInterval, UInt16LSBParser   );
        SENF_PARSER_SKIP ( 2,2); //capability flags

        SENF_PARSER_FIELD( ssidIE, WLANSSIDInfoElementParser );
        WLANSSIDInfoElementParser::value_t ssid() const { return ssidIE().value(); }

        SENF_PARSER_FIELD( supportedRatesIE, WLANSupportedRatesInfoElementParser );

        SENF_PARSER_LIST ( ieList, packetSize(), WLANGenericInfoElementParser );

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
