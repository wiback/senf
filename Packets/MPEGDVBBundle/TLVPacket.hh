// $Id: SNDUPacket.hh 423 2007-08-31 22:05:37Z g0dil $
//
// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>
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
    \brief SNDUPacket public header */

#ifndef HH_TLVPacket_
#define HH_TLVPacket_ 1

// Custom includes
#include <algorithm>
#include "../../Packets/PacketType.hh"
#include "../../Packets/ParseInt.hh"
#include "../../Packets/PacketParser.hh"

//#include "TLVPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {


    struct UnsuportedTLVPacketException : public std::exception
    { 
        virtual char const * what() const throw() { 
            return "length of length can be max. 4 bytes. Sorry.";
        }
    };

    struct Parse_TLVPacketLength 
        : public detail::packet::ParseIntOps<Parse_TLVPacketLength, boost::uint32_t>,
          public PacketParserBase
    {
#       ifndef DOXYGEN
        
        SENF_PACKET_PARSER_NO_INIT(Parse_TLVPacketLength);

        typedef Parse_Flag      <    0 > Parse_extended_length_flag;
        typedef Parse_UIntField < 1, 8 > Parse_fixed_length;

        SENF_PACKET_PARSER_DEFINE_FIXED_FIELDS(
            ((OverlayField)( extended_length_flag, Parse_extended_length_flag ))
            ((Field       )( fixed_length_field,   Parse_fixed_length         ))
        );

#       endif
        
        typedef boost::uint32_t value_type;
        
        value_type value() const {
            switch( bytes() ) {
                case 1:
                    return fixed_length_field().value();
                case 2:
                    return parse<Parse_UInt8>( 1 ).value();
                case 3:
                    return parse<Parse_UInt16>( 1 ).value();
                case 4:
                    return parse<Parse_UInt24>( 1 ).value();
                case 5:
                    return parse<Parse_UInt32>( 1 ).value();
                default:
                    throw(UnsuportedTLVPacketException()); 
            };
        }
        
        size_type bytes() const {
            if ( extended_length_flag() )
                return 1 + fixed_length_field();
            else
                return 1;
        }
        
        void init() const {
            defaultInit();
            extended_length_flag() = 0;
        }
        
    };  
        
    /** \brief parse TLVPacket Packet
    
        XXX
        
        \see TLVPacketType
     */
    struct Parse_TLVPacket : public PacketParserBase
    {
#       ifndef DOXYGEN
        
        SENF_PACKET_PARSER_INIT(Parse_TLVPacket);
        
        SENF_PACKET_PARSER_DEFINE_FIXED_FIELDS(
            ((Field)( type,   Parse_UInt32          ))
            ((Field)( length, Parse_TLVPacketLength ))
        );
        
#       endif
        
//        Parse_UInt32 type() const { 
//            return parse<Parse_UInt32>( 0 ); 
//        }
        
//        Parse_TLVPacketLength length() const {
//            return parse<Parse_TLVPacketLength>( 4 );
//        }
        
        PacketParserBase::size_type bytes() const;
        
        static const size_type init_bytes = 4+1; // 4 bytes type + 1 byte length

    };

    /** \brief TLV Packet
        
        \par Packet type (typedef):
            \ref TLVPacket

        \par Fields:
            \ref Parse_TLVPacket

        \ingroup protocolbundle_mpegdvb
     */
    struct TLVPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<TLVPacketType>
    {
        typedef PacketTypeMixin<TLVPacketType> mixin;
        typedef ConcretePacket<TLVPacketType> packet;
        typedef Parse_TLVPacket parser;

        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
        
        static void dump(packet p, std::ostream & os);
        
//        static PacketParserBase::size_type initSize();
//        static PacketParserBase::size_type initHeadSize();
    };
        
    typedef TLVPacketType::packet TLVPacket;
    
}


///////////////////////////////hh.e////////////////////////////////////////
//#include "TLVPacket.cci"
//#include "TLVPacket.ct"
//#include "TLVPacket.cti"
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
