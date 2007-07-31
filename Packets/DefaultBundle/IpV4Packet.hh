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
#include "Socket/Protocols/INet/INet4Address.hh"
#include "Packets/Packets.hh"

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
        typedef Parse_UIntField <  0,  4 > Parse_Version;
        typedef Parse_UIntField <  4,  8 > Parse_IHL;
        typedef Parse_UInt8                Parse_8bit;
        typedef Parse_UInt16               Parse_16bit;
        typedef Parse_Flag      <  0     > Parse_R;
        typedef Parse_Flag      <  1     > Parse_DF;
        typedef Parse_Flag      <  2     > Parse_MF;
        typedef Parse_UIntField <  3, 16 > Parse_Frag;
        typedef Parse_UInt32               Parse_32bit;
        typedef Parse_INet4Address         Parse_Addr;

#       ifndef DOXYGEN

        SENF_PACKET_PARSER_NO_INIT(Parse_IpV4);

        SENF_PACKET_PARSER_DEFINE_FIXED_FIELDS(
            ((OverlayField)( version,     Parse_Version ))
            ((Field       )( ihl,         Parse_IHL     ))
            ((Field       )( tos,         Parse_8bit    ))
            ((Field       )( length,      Parse_16bit   ))
            ((Field       )( identifier,  Parse_16bit   ))
            ((OverlayField)( reserved,    Parse_R       ))
            ((OverlayField)( df,          Parse_DF      ))
            ((OverlayField)( mf,          Parse_MF      ))
            ((Field       )( frag,        Parse_Frag    ))
            ((Field       )( ttl,         Parse_8bit    ))
            ((Field       )( protocol,    Parse_8bit    ))
            ((Field       )( crc,         Parse_16bit   ))
            ((Field       )( source,      Parse_Addr    ))
            ((Field       )( destination, Parse_Addr    )) );

#       else

        Parse_Version version() const;
        Parse_IHL     ihl() const;
        Parse_8bit    tos() const;
        Parse_16bit   length() const;
        Parse_16bit   identifier() const;
        Parse_R       reserved() const;
        Parse_DF      df() const;
        Parse_MF      mf() const;
        Parse_Frag    frag() const;
        Parse_8bit    ttl() const;
        Parse_8bit    protocol() const;
        Parse_16bit   crc() const;
        Parse_Addr    source() const;
        Parse_Addr    destination() const;

#       endif

        void init() {
            version() = 4;
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
