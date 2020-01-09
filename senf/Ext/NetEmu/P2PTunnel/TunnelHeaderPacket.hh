// $Id:$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fit.fraunhofer.de/license.html
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
//       Christian Niephaus <cni@berlios.de>

#ifndef HH_WiBACK_Core_Packets_TunnelHeaderPacket_
#define HH_WiBACK_Core_Packets_TunnelHeaderPacket_ 1

// Custom includes
#include <senf/Packets/Packets.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>

///////////////////////////////hh.p////////////////////////////////////////
namespace senf {
namespace emu {

    struct TunnelHeaderPacketParser : public senf::PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_BITFIELD( reserved,       15, unsigned );
        SENF_PARSER_BITFIELD( sequenceNumber, 17, unsigned );
        SENF_PARSER_FINALIZE(TunnelHeaderPacketParser);

        SENF_PARSER_INIT() {
            reserved() = 0x4711 & 0x7fff;
        }

    };

    struct TunnelHeaderPacketType
        : public senf::PacketTypeBase,
          public senf::PacketTypeMixin<TunnelHeaderPacketType>
    {
        static constexpr std::uint32_t reservedMagic = 0x4711 & 0x7fff;
 
        typedef senf::PacketTypeMixin<TunnelHeaderPacketType> mixin;
        typedef senf::ConcretePacket<TunnelHeaderPacketType> packet;
        typedef TunnelHeaderPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::initSize;
        using mixin::init;

        static factory_t nextPacketType(packet p);
        /// Dump given TunnelHeaderPacket in readable form to given output stream
        static void dump(packet p, std::ostream & os);
        static void finalize(packet p);
    };

    typedef senf::ConcretePacket<TunnelHeaderPacketType> TunnelHeaderPacket;
}}

SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION(senf::emu::TunnelHeaderPacket );

///////////////////////////////hh.e////////////////////////////////////////
#include "TunnelHeaderPacket.cci"
//#include "TunnelHeaderPacket.ct"
//#include "TunnelHeaderPacket.cti"
#endif
