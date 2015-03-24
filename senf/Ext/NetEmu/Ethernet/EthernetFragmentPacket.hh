// $Id:$
//
// Copyright (C) 2012
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fokus.fraunhofer.de.de/license.html
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
//       Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>

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
