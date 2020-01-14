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

