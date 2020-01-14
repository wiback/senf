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


/** \file
    \brief ICMPv6Packet public header */

#ifndef HH_SENF_Packets_DefaultBundle_ICMPv6Packet_
#define HH_SENF_Packets_DefaultBundle_ICMPv6Packet_ 1

// Custom includes
#include <senf/Packets/Packets.hh>
#include "Registries.hh"

//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace senf
{
    struct ICMPv6PacketParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()
        SENF_PARSER_FIELD( type,     UInt8Parser  );
        SENF_PARSER_FIELD( code,     UInt8Parser  );
        SENF_PARSER_FIELD( checksum, UInt16Parser );

        SENF_PARSER_FINALIZE( ICMPv6PacketParser );

        boost::uint16_t calcChecksum() const;
    };

    /** \brief ICMP Packet for IPv6

        \par Packet type (typedef):
            \ref ICMPv6Packet

        \par Fields:
            \ref ICMPv6PacketParser
            \image html ICMPv6Packet.png

        \par Associated registries:
            \ref ICMPTypes

        \ingroup protocolbundle_default
     */
    struct ICMPv6PacketType
        : public PacketTypeBase,
          public PacketTypeMixin<ICMPv6PacketType, ICMPTypes>
    {
        typedef PacketTypeMixin<ICMPv6PacketType, ICMPTypes> mixin;
        typedef ConcretePacket<ICMPv6PacketType> packet;
        typedef ICMPv6PacketParser parser;

        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::init;
        using mixin::initSize;

        static void dump(packet p, std::ostream & os);

        static key_t nextPacketKey(packet p) {
            return p->type();
        }

        static void finalize(packet p) {
            p->type() << key(p.next(senf::nothrow));
            p->checksum() << p->calcChecksum();
        }

        static const IPTypes::key_t ipType = 58;
    };

    /** \brief ICMPv6 packet typedef
        \ingroup protocolbundle_default
     */
    typedef ConcretePacket<ICMPv6PacketType> ICMPv6Packet;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( ICMPv6Packet );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "IPv4Packet.cci"
//#include "IPv4Packet.ct"
//#include "IPv4Packet.cti"
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

