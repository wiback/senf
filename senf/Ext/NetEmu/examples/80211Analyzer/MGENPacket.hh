// $Id:$
//
// Copyright (C) 2008
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


#ifndef HH_MGENPacket_
#define HH_MGENPacket_ 1

// Custom includes
#include <senf/Packets/Packets.hh>
#include <senf/Packets/DefaultBundle/IPv4Packet.hh>
#include <senf/Packets/DefaultBundle/IPv6Packet.hh>

///////////////////////////////hh.p////////////////////////////////////////

struct MGENPacketParser : public senf::PacketParserBase
{
#   include SENF_PARSER()

    SENF_PARSER_FIELD   ( messageSize,        senf::UInt16Parser );
    SENF_PARSER_FIELD   ( version,            senf::UInt8Parser  );
    SENF_PARSER_FIELD   ( flags,              senf::UInt8Parser  );
    SENF_PARSER_FIELD   ( flowId,             senf::UInt32Parser );
    SENF_PARSER_FIELD   ( sequenceNumber,     senf::UInt32Parser );
    SENF_PARSER_FIELD   ( txTimeSeconds,      senf::UInt32Parser );
    SENF_PARSER_FIELD   ( txTimeMicroseconds, senf::UInt32Parser );
    SENF_PARSER_FIELD   ( dstPort,            senf::UInt16Parser );
    SENF_PARSER_FIELD_RO( dstAddrType,        senf::UInt8Parser  );
    SENF_PARSER_FIELD   ( dstAddrLen,         senf::UInt8Parser  );
    SENF_PARSER_VARIANT ( dstAddr_, dstAddrType,
            (novalue( disable,      senf::VoidPacketParser   ))
            (     id( dstAddrV4,    senf::INet4AddressParser ))
            (     id( dstAddrV6,    senf::INet6AddressParser )) );

    SENF_PARSER_FINALIZE(MGENPacketParser);
};

struct MGENPacketType
    : public senf::PacketTypeBase,
      public senf::PacketTypeMixin<MGENPacketType>
{
    typedef senf::PacketTypeMixin<MGENPacketType> mixin;
    typedef senf::ConcretePacket<MGENPacketType> packet;
    typedef MGENPacketParser parser;

    using mixin::nextPacketRange;
    using mixin::initSize;
    using mixin::init;

    static void dump(packet p, std::ostream & os);
};

typedef senf::ConcretePacket<MGENPacketType> MGENPacket;


///////////////////////////////hh.p////////////////////////////////////////
#endif

