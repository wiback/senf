// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief IpV6Packet public header */

#ifndef HH_IpV6Packet_
#define HH_IpV6Packet_ 1

// Custom includes
#include "../../Socket/Protocols/INet/INet6Address.hh"
#include "../../Packets/Packets.hh"
#include "IpV4Packet.hh"

//#include "IpV6Packet.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Parse an IpV6 address
        
        \see INet6Address
     */
    struct Parse_INet6Address : public PacketParserBase
    {
        Parse_INet6Address(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}

        ///////////////////////////////////////////////////////////////////////////

        typedef INet6Address value_type;
        static const size_type fixed_bytes = 16u;

        value_type value() const { return value_type::from_data(i()); }
        void value(value_type const & v) { std::copy(v.begin(), v.end(), i()); }
        operator value_type() { return value(); }
        byte & operator[](size_type index) { return *boost::next(i(),index); }
        Parse_INet6Address const & operator= (value_type const & other) 
            { value(other); return *this; }
    };

    /** \brief Parse an IpV6 packet

        \see IpV6PacketType \n
            <a href="http://tools.ietf.org/html/rfc2460">RFC 2460</a>
     */
    struct Parse_IpV6 : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSE_BITFIELD( version,       4, unsigned );
        SENF_PARSE_BITFIELD( trafficClass,  8, unsigned );
        SENF_PARSE_BITFIELD( flowLabel,    20, unsigned );

        SENF_PARSE_FIELD( length,       Parse_UInt16       );
        SENF_PARSE_FIELD( nextHeader,   Parse_UInt8        );
        SENF_PARSE_FIELD( hopLimit,     Parse_UInt8        );
        SENF_PARSE_FIELD( source,       Parse_INet6Address );
        SENF_PARSE_FIELD( destination,  Parse_INet6Address );

        SENF_PARSER_INIT() {
            version() = 6;
        }

        SENF_PARSER_FINALIZE(Parse_IpV6);
    };

    /** \brief IpV6 packet

        \par Packet type (typedef):
            \ref IpV6Packet
        
        \par Fields:
            \ref Parse_IpV6

        \par Associated registries:
            \ref IpTypes

        \ingroup protocolbundle_default
     */
    struct IpV6PacketType
        : public PacketTypeBase,
          public PacketTypeMixin<IpV6PacketType, IpTypes>
    {
        typedef PacketTypeMixin<IpV6PacketType, IpTypes> mixin;
        typedef ConcretePacket<IpV6PacketType> packet;
        typedef Parse_IpV6 parser;

        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::initSize;
        using mixin::init;

        static registry_key_t nextPacketKey(packet p) 
            { return p->nextHeader(); }
        
        static void dump(packet p, std::ostream & os);

        static void finalize(packet p);
    };

    /** \brief IpV6 packet typedef */
    typedef IpV6PacketType::packet IpV6Packet;

    ///@}
}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#ifndef SENF_PACKETS_DECL_ONLY
//#include "IpV6Packet.cci"
//#include "IpV6Packet.ct"
//#include "IpV6Packet.cti"
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
