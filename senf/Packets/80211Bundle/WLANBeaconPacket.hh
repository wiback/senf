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
    \brief 802.11 WLANPacket public header */

#ifndef HH_SENF_Packets_80211Bundle_WLANBeaconPacket_
#define HH_SENF_Packets_80211Bundle_WLANBeaconPacket_ 1

#include <senf/Packets/Packets.hh>


namespace senf
{

    /** \brief 802.11 Beacon Frame parser
        (see IEEE 802.11-2007 standard - Chapter 7.2.3.1 Frame formats)
        <b>Re-ordering of bits due to host byte order</b>

        Frame base class.
     */

//    struct WLANBeaconPacketParser : public senf::PacketParserBase
//    {
//        #include SENF_PARSER();
//        SENF_PARSER_FIELD( elementId, UInt8Parser);
//        SENF_PARSER_FIELD( length, UInt8Parser);
//        
//    }


    struct WLANBeaconPacketParser : public senf::PacketParserBase
    {
    #   include SENF_PARSER()

        SENF_PARSER_FIELD(timestamp, UInt64LSBParser);
        SENF_PARSER_FIELD(beaconInterval, UInt16LSBParser);
        SENF_PARSER_SKIP(2,2); //capability flags

        //SSID element
//        SENF_PARSER_FIELD()
        
        SENF_PARSER_FINALIZE(WLANBeaconPacketParser);
        
    };

 
    /** \brief WLAN Beacon frame packet

        \par Packet type (typedef):
            \refWLANBeaconPacket

        \par Fields:
            \ref WLANBeaconPacket_MgtFrameParser
            \image html WLANBeaconPacket.png

        \ingroup protocolbundle_80211
     */
    struct WLANBeaconPacketType
        : public senf::PacketTypeBase,
          public senf::PacketTypeMixin<WLANBeaconPacketType>
    {
        typedef senf::PacketTypeMixin<WLANBeaconPacketType> mixin;
        typedef senf::ConcretePacket<WLANBeaconPacketType> packet;
        typedef WLANBeaconPacketParser parser;

        using mixin::init;
        using mixin::initSize;
        using senf::PacketTypeBase::nextPacketRange;

        static void dump(packet p, std::ostream &os);
    };

    typedef ConcretePacket<WLANBeaconPacketType> WLANBeaconPacket;


}

#endif /* HH_SENF_Packets_80211Bundle_WLANBeaconPacket_ */


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
