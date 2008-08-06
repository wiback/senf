// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Philipp Batroff <Philipp.Batroff@fokus.fraunhofer.de>
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

#ifndef HH_ICMPV6Packet
#define HH_ICMPV6Packet

// Custom includes
#include "../../Packets/Packets.hh"
#include "../../Packets/DefaultBundle/IPv6Packet.hh"

namespace senf{
    struct ICMPV6PacketParser : public senf::PacketParserBase
    {
    #       include SENF_FIXED_PARSER()
            SENF_PARSER_FIELD    ( type, senf::UInt8Parser );
            SENF_PARSER_FIELD    ( code, senf::UInt8Parser );
            SENF_PARSER_PRIVATE_FIELD ( checksum, senf::UInt16Parser);
    
            SENF_PARSER_FINALIZE ( ICMPV6PacketParser );
    
            void calcChecksum() const;
            boost::uint16_t checksumOutput() const
                { return this->checksum();}
    };
    
        struct ICMPTypes {
            // ICMP type registry
            typedef boost::uint16_t key_t;
        };
    
    struct ICMPV6PacketType 
            : public senf::PacketTypeBase,
            public senf::PacketTypeMixin<ICMPV6PacketType, ICMPTypes>
    {
        typedef senf::PacketTypeMixin<ICMPV6PacketType, ICMPTypes> mixin;
        typedef senf::ConcretePacket<ICMPV6PacketType> packet;
        typedef ICMPV6PacketParser parser;
        
        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::init;
        using mixin::initSize;
        
        static void dump(packet p, std::ostream & os);
        static senf::IpTypes::key_t nextPacketKey(packet p) { 
            return p->type();
        }
        static void finalize(packet p) {
            p->calcChecksum();
            p->type() << key(p.next(senf::nothrow));
        }
    };
    
    typedef ICMPV6PacketType::packet ICMPv6Packet;
}
#endif
