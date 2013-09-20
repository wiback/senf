// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
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
//   Philipp Batroff <pug@berlios.de>

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
        SENF_PARSER_FIELD ( type,     UInt8Parser  );
        SENF_PARSER_FIELD ( code,     UInt8Parser  );
        SENF_PARSER_FIELD ( checksum, UInt16Parser );

        SENF_PARSER_FINALIZE ( ICMPv6PacketParser );

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

