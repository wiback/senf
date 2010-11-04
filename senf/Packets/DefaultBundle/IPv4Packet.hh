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

#ifndef HH_SENF_Packets_DefaultBundle_IPv4Packet_
#define HH_SENF_Packets_DefaultBundle_IPv4Packet_ 1

// Custom includes
#include <senf/Socket/Protocols/INet/INet4Address.hh>
#include <senf/Packets/Packets.hh>

//#include "IPv4Packet.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Parse in IPv4 address

        \see INet4Address
     */
    struct INet4AddressParser
        : public ValueParserBase<INet4AddressParser, INet4Address, 4u>
    {
        INet4AddressParser(data_iterator i, state_type s) : Base(i,s) {}

        value_type value() const { return value_type::from_data(i()); }
        void value(value_type const & v) { std::copy(v.begin(), v.end(), i()); }

        using Base::operator=;
    };

    /** \brief Parse an IPv4 packet

        Parser implementing the IPv4 header.

        \image html IPv4Packet.png

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

        SENF_PARSER_PRIVATE_BITFIELD( reserved,  1, bool     );
        SENF_PARSER_BITFIELD        ( df,        1, bool     );
        SENF_PARSER_BITFIELD        ( mf,        1, bool     );
        SENF_PARSER_BITFIELD        ( frag,     13, unsigned );

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

        boost::uint16_t calcChecksum() const; ///< calculate header checksum
                                              /**< calculate and return the checksum of the header
                                                   \see \ref senf::IpChecksum */

        bool validateChecksum() const {
            return checksum() == calcChecksum();
        }                               ///< validate header checksum
                                        /**< return \c true if the \ref checksum() "checksum"
                                             field is equal to the \ref calcChecksum()
                                             "calculated checksum" */
    };

    /** \brief IP protocol number registry

        This registeres packets with their IP protocol number.

        \see <a href="http://www.iana.org/assignments/protocol-numbers">Protocol numbers</a> \n
            PacketRegistry
     */
    struct IpTypes {
        typedef boost::uint8_t key_t;
    };

    /** \brief IPv4 packet

        \par Packet type (typedef):
            \ref IPv4Packet

        \par Fields:
            see \ref IPv4PacketParser

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
            <td>R</td>
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

        \par Associated registries:
            \ref IpTypes

        \par Finalize action:
            \copydetails finalize()

        \ingroup protocolbundle_default
     */
    struct IPv4PacketType
        : public PacketTypeBase,
          public PacketTypeMixin<IPv4PacketType, IpTypes>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<IPv4PacketType, IpTypes> mixin;
#endif
        typedef ConcretePacket<IPv4PacketType> packet;  ///< IPv4 packet typedef
        typedef IPv4PacketParser parser;                ///< typedef to the parser of IPv4 packet

        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::initSize;
        using mixin::init;

        static key_t nextPacketKey(packet p)
            { return p->protocol(); }

        /** \brief Dump given IPv4Packet in readable form to given output stream */
        static void dump(packet p, std::ostream & os);

        static void finalize(packet p); ///< Finalize packet.
                                        /**< \li set \ref IPv4PacketParser::length() "length"
                                               from payload size
                                             \li set \ref IPv4PacketParser::protocol() "protocol"
                                               from type of next packet if found in \ref IpTypes
                                             \li calculate and set
                                               \ref IPv4PacketParser::checksum() "checksum" */
    };

    /** \brief IPv4 packet typedef
        \ingroup protocolbundle_default
     */
    typedef ConcretePacket<IPv4PacketType> IPv4Packet;
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
