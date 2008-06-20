// $Id: EthernetPacket.hh 299 2007-07-10 21:23:49Z g0dil $
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
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
    \brief IPv6Packet public header */

#ifndef HH_IPv6Packet_
#define HH_IPv6Packet_ 1

// Custom includes
#include "../../Socket/Protocols/INet/INet6Address.hh"
#include "../../Packets/Packets.hh"
#include "IPv4Packet.hh"

//#include "IPv6Packet.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Parse an IPv6 address
        
        \see INet6Address
     */
    struct INet6AddressParser : public PacketParserBase
    {
        INet6AddressParser(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef INet6Address value_type;
        static const size_type fixed_bytes = 16u;

        value_type value() const { return value_type::from_data(i()); }
        void value(value_type const & v) { std::copy(v.begin(), v.end(), i()); }
        operator value_type() const { return value(); }
        byte & operator[](size_type index) const { return *boost::next(i(),index); }
        INet6AddressParser const & operator= (value_type const & other) 
            { value(other); return *this; }
    };

    /** \brief Parse an IPv6 packet

        \see IPv6PacketType \n
            <a href="http://tools.ietf.org/html/rfc2460">RFC 2460</a>
     */
    struct IPv6PacketParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_BITFIELD( version,       4, unsigned );
        SENF_PARSER_BITFIELD( trafficClass,  8, unsigned );
        SENF_PARSER_BITFIELD( flowLabel,    20, unsigned );

        SENF_PARSER_FIELD( length,       UInt16Parser       );
        SENF_PARSER_FIELD( nextHeader,   UInt8Parser        );
        SENF_PARSER_FIELD( hopLimit,     UInt8Parser        );
        SENF_PARSER_FIELD( source,       INet6AddressParser );
        SENF_PARSER_FIELD( destination,  INet6AddressParser );

        SENF_PARSER_INIT() {
            version() = 6;
        }

        SENF_PARSER_FINALIZE(IPv6PacketParser);
    };

    /** \brief IPv6 packet

        \par Packet type (typedef):
            \ref IPv6Packet
        
        \par Fields:
            \ref IPv6PacketParser

        \par Associated registries:
            \ref IpTypes

        \par Finalize action:
            Set \a length from payload size\n
            Set \a nextHeader from type of next packet if found in \ref IpTypes

        \ingroup protocolbundle_default
     */
    struct IPv6PacketType
        : public PacketTypeBase,
          public PacketTypeMixin<IPv6PacketType, IpTypes>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<IPv6PacketType, IpTypes> mixin;
        typedef ConcretePacket<IPv6PacketType> packet;
        typedef IPv6PacketParser parser;
#endif
        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::initSize;
        using mixin::init;

        static key_t nextPacketKey(packet p) 
            { return p->nextHeader(); }
        
        static void dump(packet p, std::ostream & os);

        static void finalize(packet p);
    };

    /** \brief IPv6 packet typedef */
    typedef ConcretePacket<IPv6PacketType> IPv6Packet;

    ///@}
}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#ifndef SENF_PACKETS_DECL_ONLY
//#include "IPv6Packet.cci"
//#include "IPv6Packet.ct"
//#include "IPv6Packet.cti"
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
