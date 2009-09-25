// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Philipp Batroff <pug@berlios.de>
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
    \brief ICMPv6Packet public header */

#ifndef HH_SENF_Packets_DefaultBundle_ICMPv6Packet_
#define HH_SENF_Packets_DefaultBundle_ICMPv6Packet_ 1

// Custom includes
#include <senf/Packets/Packets.hh>

//#include "ICMPv6Packet.mpp"
///////////////////////////////hh.p////////////////////////////////////////
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
    
    struct ICMPTypes {
        // ICMP type registry
        typedef boost::uint8_t key_t;
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
    };
    
    typedef ConcretePacket<ICMPv6PacketType> ICMPv6Packet;
}

///////////////////////////////hh.e////////////////////////////////////////
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

