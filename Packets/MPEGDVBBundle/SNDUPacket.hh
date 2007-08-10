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
    \brief SNDUPacket public header */

#ifndef HH_SNDUPacket_
#define HH_SNDUPacket_ 1

// Custom includes
#include <algorithm>
#include <boost/crc.hpp>
#include "Packets/PacketType.hh"
#include "Packets/ParseInt.hh"
#include "Packets/PacketRegistry.hh"
#include "Packets/PacketParser.hh"
#include "Packets/DefaultBundle/EthernetPacket.hh"

//#include "SNDUPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief parse ULE SNDU Packet
    
        Parser implementing the header and trailer of a ULE SNDU Packet
        
        \see SNDUPacketType
     */
    struct Parse_SNDUPacket : public PacketParserBase
    {
#       ifndef DOXYGEN
        
        SENF_PACKET_PARSER_NO_INIT(Parse_SNDUPacket);

#       endif
        
        typedef Parse_Flag      <     0 > Parse_daaf;  // Destination Address Absent Field
        typedef Parse_UIntField < 1, 16 > Parse_length;
                
        Parse_daaf d_bit() const {
            return parse<Parse_daaf>( 0 );
        }
        Parse_length length() const {
            return parse<Parse_length>( 0 );
        }
        Parse_UInt16 type() const {
            return parse<Parse_UInt16>( 2 );
        }
        Parse_MAC destination() const {
            BOOST_ASSERT( ! d_bit() );
            return parse<Parse_MAC>( 4 );
        }
        Parse_UInt32 crc() const { 
            return parse<Parse_UInt32>( data().size()-4 ); 
        }
        
        void init() const {
            defaultInit();
            d_bit() = false;
        }
        
        PacketParserBase::size_type bytes() const;
        
        static const size_type init_bytes = 2+2+4; // D-Bit + 15 bits length + 16 bits type field + 32 bits crc

        boost::uint32_t calcCrc() const;
    };

    
    struct ULEExtHeaderTypes {
        typedef boost::uint16_t key_t;
    };
    
    /** \brief ULE SNDU Packet
        
        \par Packet type (typedef):
            \ref SNDUPacket

        \par Fields:
            \ref Parse_SNDUPacket

        \ingroup protocolbundle_mpegdvb
     */
    struct SNDUPacketType
        : public PacketTypeBase
//          public PacketTypeMixin<SNDUPacketType, ULEExtHeaderTypes>
    {
//        typedef PacketTypeMixin<SNDUPacketType, ULEExtHeaderType> mixin;
        typedef ConcretePacket<SNDUPacketType> packet;
        typedef Parse_SNDUPacket parser;

//        using mixin::nextPacketRange;
//        using mixin::nextPacketType;
//        using mixin::init;
        
//        static registry_key_t nextPacketKey(packet p);
        
        static void init(packet p);

        static factory_t nextPacketType(packet p);
        
        static optional_range nextPacketRange(packet p);
        
        static void dump(packet p, std::ostream & os);
        
        static PacketParserBase::size_type initSize();
        
        static PacketParserBase::size_type initHeadSize();
    };
        
    typedef SNDUPacketType::packet SNDUPacket;
    
    typedef boost::crc_optimal<32, 0x04C11DB7, 0xFFFFFFFF, 0, false, false> ule_crc32;


    /*!
     \def ULE_END_INDICATOR 
         ULE End Indicator; indicates to the receiver that there are no 
         further SNDUs present within the current TS packet.
    */
#   define ULE_END_INDICATOR 0xffff
}


///////////////////////////////hh.e////////////////////////////////////////
//#include "SNDUPacket.cci"
//#include "SNDUPacket.ct"
//#include "SNDUPacket.cti"
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
