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
#include "../../Packets/Packets.hh"
#include "../DefaultBundle/EthernetPacket.hh"

//#include "SNDUPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief parse ULE SNDU Packet
    
        Parser implementing the header and trailer of a ULE SNDU Packet
        
        \see SNDUPacketType
     */
    struct Parse_SNDUPacket : public PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_BITFIELD     ( d_bit       ,  1 , bool     );
        SENF_PARSER_BITFIELD     ( length      , 15 , unsigned );

        SENF_PARSER_FIELD        ( type        , Parse_UInt16  );

        // This field only exists, if d_bit() is *not* set. New SNDUPackets are created with d_bit()
        // set to 0, they have destination. We set the size of this field depending on the value of
        // d_bit(), the init_bytes value is set to 6 bytes (the size of a MAC address)
        SENF_PARSER_CUSTOM_FIELD ( destination , Parse_MAC     , d_bit() ? 0 : 6 , 6 ) {
            BOOST_ASSERT( ! d_bit() );
            return parse<destination_t>( destination_offset() );
        }

        // This field is placed at the end of the parser. It is therefore only considered for
        // calculating init_bytes but not for calculating bytes()
        SENF_PARSER_CUSTOM_FIELD ( crc         , Parse_UInt32  , 0 , 4 ) {
            return parse<crc_t>( data().size() - 4 );
        }

        SENF_PARSER_FINALIZE( Parse_SNDUPacket );
        
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
