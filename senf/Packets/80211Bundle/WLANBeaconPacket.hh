// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Christian Niephaus <cni@berlios.de>
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief 802.11 WLAN Beacon Packet public header */

#ifndef HH_SENF_Packets_80211Bundle_WLANBeaconPacket_
#define HH_SENF_Packets_80211Bundle_WLANBeaconPacket_ 1

// Custom includes
#include "InformationElements.hh"

///////////////////////////////hh.p////////////////////////////////////////
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

        static void dump(packet p, std::ostream &os);
    };

    /** \brief WLAN Beacon frame packet typedef
        \ingroup protocolbundle_80211
     */
    typedef ConcretePacket<WLANBeaconPacketType> WLANBeaconPacket;

}

///////////////////////////////hh.e////////////////////////////////////////
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
