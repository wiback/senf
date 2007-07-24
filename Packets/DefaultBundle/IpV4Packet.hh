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

#ifndef HH_IpV4Packet_
#define HH_IpV4Packet_ 1

// Custom includes
#include "Packets/Packets.hh"

//#include "IpV4Packet.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

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
            ((Field       )( source,      Parse_32bit   ))
            ((Field       )( destination, Parse_32bit   )) );

#       else

        Parse_Version version();
        Parse_IHL ihl();
        Parse_8bit tos();
        Parse_16bit length();
        Parse_16bit identifier();
        Parse_R reserved();
        Parse_DF df();
        Parse_MF mf();
        Parse_Frag frag();
        Parse_8bit ttl();
        Parse_8bit protocol();
        Parse_16bit crc();
        Parse_32bit source();
        Parse_32bit destination();

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
