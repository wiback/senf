//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
