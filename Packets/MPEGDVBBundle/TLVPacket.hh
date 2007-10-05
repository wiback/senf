// $Id$
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
    \brief TLVPacket public header */

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

    class Parse_TLVPacketLength 
        : public detail::packet::ParseIntOps<Parse_TLVPacketLength, boost::uint32_t>,
          public PacketParserBase
    {
    public:
#       ifndef DOXYGEN
        
        SENF_PACKET_PARSER_NO_INIT(Parse_TLVPacketLength);

#       endif

        typedef boost::uint32_t value_type;
    
        value_type value() const;

        void value(value_type const & v);
        
        Parse_TLVPacketLength const & operator= (value_type other);
            
        static const size_type init_bytes = 1;

        size_type bytes() const;
            
        void init() const;

    private:
        typedef Parse_Flag      <    0 > Parse_extended_length_flag;
        typedef Parse_UIntField < 1, 8 > Parse_fixed_length;

        Parse_extended_length_flag extended_length_flag() const {
            return parse<Parse_extended_length_flag>( 0 );
        }

        Parse_fixed_length fixed_length_field() const {
            return parse<Parse_fixed_length>( 0 );
        }
        
        void resize(size_type size);
    };  
        
    /** \brief parse TLVPacket Packet
    
        XXX
        
        \see TLVPacketType
     */
    struct Parse_TLVPacket : public PacketParserBase
    {
#       ifndef DOXYGEN
        
        SENF_PACKET_PARSER_INIT(Parse_TLVPacket);
        
        SENF_PACKET_PARSER_DEFINE_FIELDS(
            ((Field)( type,   Parse_UInt32          ))
            ((Field)( length, Parse_TLVPacketLength ))
        );
        
#       endif
    };

    /** \brief TLV Packet
        
        \par Packet type (typedef):
            \ref TLVPacket

        \par Fields:
            \ref Parse_TLVPacket

        \ingroup protocolbundle_mpegdvb
     */
    struct TLVPacketType
        : public PacketTypeBase
    {
        typedef ConcretePacket<TLVPacketType> packet;
        typedef Parse_TLVPacket parser;

        static optional_range nextPacketRange(packet p);
        static void init(packet p);
        static size_type initSize();
        static void finalize(packet p);
        static void dump(packet p, std::ostream & os);
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
