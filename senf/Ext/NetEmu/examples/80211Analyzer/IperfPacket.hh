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


#ifndef HH_IperfPacket_
#define HH_IperfPacket_ 1

// Custom includes
#include <senf/Packets/Packets.hh>

///////////////////////////////hh.p////////////////////////////////////////

struct IperfUDPPacketParser : public senf::PacketParserBase
{
#   include SENF_FIXED_PARSER()

    SENF_PARSER_FIELD   ( id,      senf::Int32Parser  );
    SENF_PARSER_FIELD   ( tv_sec,  senf::UInt32Parser );
    SENF_PARSER_FIELD   ( tv_usec, senf::UInt32Parser );
    SENF_PARSER_FINALIZE( IperfUDPPacketParser        );
};

struct IperfUDPPacketType
    : public senf::PacketTypeBase,
      public senf::PacketTypeMixin<IperfUDPPacketType>
{
    typedef senf::PacketTypeMixin<IperfUDPPacketType> mixin;
    typedef senf::ConcretePacket<IperfUDPPacketType> packet;
    typedef IperfUDPPacketParser parser;

    using mixin::nextPacketRange;
    using mixin::initSize;
    using mixin::init;

    static void dump(packet p, std::ostream & os);
};

typedef senf::ConcretePacket<IperfUDPPacketType> IperfUDPPacket;


///////////////////////////////hh.e////////////////////////////////////////
#endif
