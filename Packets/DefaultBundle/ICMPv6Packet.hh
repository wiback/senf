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

#ifndef HH_SENF_Packets_DefaultBundle_ICMPv6Packet_
#define HH_SENF_Packets_DefaultBundle_ICMPv6Packet_ 1

// Custom includes
#include "../../Packets/Packets.hh"
#include "../../Packets/DefaultBundle/IPv6Packet.hh"

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
        typedef boost::uint16_t key_t;
    };
    
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
        
        static IpTypes::key_t nextPacketKey(packet p) { 
            return p->type();
        }
        
        static void finalize(packet p) {
            p->checksum() << p->calcChecksum();
            p->type() << key(p.next(senf::nothrow));
        }
    };
    
    typedef ConcretePacket<ICMPv6PacketType> ICMPv6Packet;
}

#endif