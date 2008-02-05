// $Id$
//
// Copyright (C) 2006
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
    \brief IPv4Packet public header */

#ifndef HH_IPv4Packet_
#define HH_IPv4Packet_ 1

// Custom includes
#include "../../Socket/Protocols/INet/INet4Address.hh"
#include "../../Packets/Packets.hh"

//#include "IPv4Packet.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Parse in IPv4 address

        \see INet4Address
     */
    struct INet4AddressParser : public PacketParserBase
    {
        INet4AddressParser(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef INet4Address value_type;
        static const size_type fixed_bytes = 4u;

        value_type value() const { return value_type::from_data(i()); }
        void value(value_type const & v) { std::copy(v.begin(), v.end(), i()); }
        operator value_type() { return value(); }
        byte & operator[](size_type index) { return *boost::next(i(),index); }
        INet4AddressParser const & operator= (value_type const & other) 
            { value(other); return *this; }
    };

    /** \brief Parse an IPv4 packet

        Parser implementing the IPv4 header.
        
        \see IPv4PacketType \n
            <a href="http://tools.ietf.org/html/rfc791">RFC 791</a>

        \todo Implement options
     */
    struct IPv4PacketParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_BITFIELD( version,   4, unsigned );
        SENF_PARSER_BITFIELD( ihl,       4, unsigned );

        SENF_PARSER_FIELD( tos,         UInt8Parser        );
        SENF_PARSER_FIELD( length,      UInt16Parser       );
        SENF_PARSER_FIELD( identifier,  UInt16Parser       );

        SENF_PARSER_BITFIELD( reserved,  1, bool     );
        SENF_PARSER_BITFIELD( df,        1, bool     );
        SENF_PARSER_BITFIELD( mf,        1, bool     );
        SENF_PARSER_BITFIELD( frag,     13, unsigned );

        SENF_PARSER_FIELD( ttl,         UInt8Parser        );
        SENF_PARSER_FIELD( protocol,    UInt8Parser        );
        SENF_PARSER_FIELD( checksum,    UInt16Parser       );
        SENF_PARSER_FIELD( source,      INet4AddressParser );
        SENF_PARSER_FIELD( destination, INet4AddressParser );

        SENF_PARSER_INIT() {
            version() = 4;
            // We don't support option headers at the moment ...
            ihl() = 5;
        }

        SENF_PARSER_FINALIZE(IPv4PacketParser);
        
        boost::uint16_t calcChecksum() const;

        bool validateChecksum() const {
            return checksum() == calcChecksum();
        }
    };

    /** \brief IP protocol number registry

        This registeres packets with their IP protocol number.

        \see <a href="http://www.iana.org/assignments/protocol-numbers">Protocol numbers</a> \n
            PacketRegistry
     */
    struct IpTypes {
        typedef boost::uint16_t key_t;
    };

    /** \brief IPv4 packet

        <table class="packet" cellpadding="5" cellspacing="1" border="1">
          <tr>
            <th width="12%">0</th> <th width="12%">4</th> <th width="12%">8</th>
            <th width="12%">12</th> <th width="3%">16</th>
            <th width="3%"></th> <th width="3%"></th> <th width="3%"></th>
            <th width="12%">20</th> <th width="12%">24</th> <th width="6%">28</th>
            <th style="text-align:right" width="6%">31</th>
          </tr><tr>
            <td>\ref IPv4PacketParser::version() "Version"</td>
            <td>\ref IPv4PacketParser::ihl() "IHL"</td>
            <td colspan="2">\ref IPv4PacketParser::tos() "TOS"</td>
            <td colspan="8">\ref IPv4PacketParser::length() "Length"</td> 
          </tr><tr>
            <td colspan="4">\ref IPv4PacketParser::identifier() "Identifier"</td>
            <td>\ref IPv4PacketParser::reserved() "R"</td>
            <td>\ref IPv4PacketParser::df() "DF"</td>
            <td>\ref IPv4PacketParser::mf() "MF"</td>
            <td colspan="5">\ref IPv4PacketParser::frag() "Fragment Offset"</td>
          </tr><tr>
            <td colspan="2">\ref IPv4PacketParser::ttl() "Time to Live (ttl)"</td>
            <td colspan="2">\ref IPv4PacketParser::protocol() "Protocol"</td>
            <td colspan="8">\ref IPv4PacketParser::checksum() "Header Checksum"</td>
          </tr><tr>
            <td colspan="12">\ref IPv4PacketParser::source() "Source Address"</td>
          </tr><tr>
            <td colspan="12">\ref IPv4PacketParser::destination() "Destination Address"</td>
          </tr>
        </table>
        
        \par Packet type (typedef):
            \ref IPv4Packet

        \par Fields:
            \ref IPv4PacketParser

        \par Associated registries:
            \ref IpTypes

        \par Finalize action:
            Set \a length from payload size\n
            Set \a protocol from type of next packet if found in \ref IpTypes\n
            Calculate \a checksum

        \ingroup protocolbundle_default
     */
    struct IPv4PacketType
        : public PacketTypeBase,
          public PacketTypeMixin<IPv4PacketType, IpTypes>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<IPv4PacketType, IpTypes> mixin;
        typedef ConcretePacket<IPv4PacketType> packet;
        typedef IPv4PacketParser parser;
#endif
        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::initSize;
        using mixin::init;

        static key_t nextPacketKey(packet p) 
            { return p->protocol(); }

        static void dump(packet p, std::ostream & os);
        static void finalize(packet p);
    };
        
    /** \brief IPv4 packet typedef */
    typedef ConcretePacket<IPv4PacketType> IPv4Packet;
}


///////////////////////////////hh.e////////////////////////////////////////
#endif
#ifndef SENF_PACKETS_DECL_ONLY
//#include IPv4Packet.cci"
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
