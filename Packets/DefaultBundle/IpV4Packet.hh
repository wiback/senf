// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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
    \brief IpV4Packet public header */

#ifndef HH_IpV4Packet_
#define HH_IpV4Packet_ 1

// Custom includes
#include "../../Socket/Protocols/INet/INet4Address.hh"
#include "../../Packets/Packets.hh"

//#include "IpV4Packet.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Parse in IpV4 address

        \see INet4Address
     */
    struct Parse_INet4Address : public PacketParserBase
    {
        Parse_INet4Address(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef INet4Address value_type;
        static const size_type fixed_bytes = 4u;

        value_type value() const { return value_type::from_data(i()); }
        void value(value_type const & v) { std::copy(v.begin(), v.end(), i()); }
        operator value_type() { return value(); }
        byte & operator[](size_type index) { return *boost::next(i(),index); }
        Parse_INet4Address const & operator= (value_type const & other) 
            { value(other); return *this; }
    };

    /** \brief Parse an IpV4 packet

        Parser implementing the IpV4 header. The fields implemented are:

        \see IpV4PacketType \n
            <a href="http://tools.ietf.org/html/rfc791">RFC 791</a>

        \todo Implement options
     */
    struct Parse_IpV4 : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_BITFIELD( version,   4, unsigned );
        SENF_PARSER_BITFIELD( ihl,       4, unsigned );

        SENF_PARSER_FIELD( tos,         Parse_UInt8        );
        SENF_PARSER_FIELD( length,      Parse_UInt16       );
        SENF_PARSER_FIELD( identifier,  Parse_UInt16       );

        SENF_PARSER_BITFIELD( reserved,  1, bool     );
        SENF_PARSER_BITFIELD( df,        1, bool     );
        SENF_PARSER_BITFIELD( mf,        1, bool     );
        SENF_PARSER_BITFIELD( frag,     13, unsigned );

        SENF_PARSER_FIELD( ttl,         Parse_UInt8        );
        SENF_PARSER_FIELD( protocol,    Parse_UInt8        );
        SENF_PARSER_FIELD( checksum,    Parse_UInt16       );
        SENF_PARSER_FIELD( source,      Parse_INet4Address );
        SENF_PARSER_FIELD( destination, Parse_INet4Address );

        SENF_PARSER_INIT() {
            version() = 4;
            // We don't support option headers at the moment ...
            ihl() = 5;
        }

        SENF_PARSER_FINALIZE(Parse_IpV4);
        
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

    /** \brief IpV4 packet
        
        \par Packet type (typedef):
            \ref IpV4Packet

        \par Fields:
            \ref Parse_IpV4

        \par Associated registries:
            \ref IpTypes

        \ingroup protocolbundle_default
     */
    struct IpV4PacketType
        : public PacketTypeBase,
          public PacketTypeMixin<IpV4PacketType, IpTypes>
    {
        typedef PacketTypeMixin<IpV4PacketType, IpTypes> mixin;
        typedef ConcretePacket<IpV4PacketType> packet;
        typedef Parse_IpV4 parser;

        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::initSize;
        using mixin::init;

        static registry_key_t nextPacketKey(packet p) 
            { return p->protocol(); }

        static void dump(packet p, std::ostream & os);
        static void finalize(packet p);
    };
        
    /** \brief IpV4 packet typedef */
    typedef IpV4PacketType::packet IpV4Packet;
}


///////////////////////////////hh.e////////////////////////////////////////
#endif
#ifndef SENF_PACKETS_DECL_ONLY
//#include IpV4Packet.cci"
//#include "IpV4Packet.ct"
//#include "IpV4Packet.cti"
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
