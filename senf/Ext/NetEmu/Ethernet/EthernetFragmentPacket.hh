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


#ifndef HH_WiBACK_Core_Packets_EthernetFragmentPacket_
#define HH_WiBACK_Core_Packets_EthernetFragmentPacket_ 1

// Custom includes
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>

///////////////////////////////hh.p////////////////////////////////////////
namespace senf {
namespace emu {

    struct EthernetFragmentPacketParser : public senf::PacketParserBase
    {
#       include SENF_FIXED_PARSER()
        SENF_PARSER_BITFIELD( fragmentNr,   7, unsigned        );
        SENF_PARSER_BITFIELD( moreFragment, 1, bool            );
        SENF_PARSER_FIELD   ( size,         senf::UInt16Parser );
        SENF_PARSER_FIELD   ( type_length,  senf::UInt16Parser );
        SENF_PARSER_FINALIZE( EthernetFragmentPacketParser     );
    };

    struct EthernetFragmentPacketType
        : public senf::PacketTypeBase,
          public senf::PacketTypeMixin<EthernetFragmentPacketType>
    {
        typedef senf::PacketTypeMixin<EthernetFragmentPacketType> mixin;
        typedef senf::ConcretePacket<EthernetFragmentPacketType> packet;
        typedef EthernetFragmentPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::initSize;
        using mixin::init;

        static void dump(packet p, std::ostream & os);
    };

    typedef senf::ConcretePacket<EthernetFragmentPacketType> EthernetFragmentPacket;
}}

SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( senf::emu::EthernetFragmentPacket );

///////////////////////////////hh.e////////////////////////////////////////
//#include "EthernetFragmentPacket.cci"
//#include "EthernetFragmentPacket.ct"
//#include "EthernetFragmentPacket.cti"
#endif
