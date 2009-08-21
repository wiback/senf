// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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

#ifndef HH_SENF_Packets_MPEGDVBBundle_SNDUPacket_
#define HH_SENF_Packets_MPEGDVBBundle_SNDUPacket_ 1

// Custom includes
#include <algorithm>
#include <boost/crc.hpp>
#include <senf/Packets/Packets.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>

//#include "SNDUPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief parse ULE SNDU Packet
    
        Parser implementing the header and trailer of a ULE SNDU Packet
        
        \see SNDUPacketType
     */
    struct SNDUPacketParser : public PacketParserBase
    {
#       include SENF_PARSER()

        /* We first define the fields as they appear in the field. Some of the fields are declared
           private. We provide custom accessors for those fields further down. */

        SENF_PARSER_PRIVATE_BITFIELD ( d_bit_       ,  1 , unsigned );
        SENF_PARSER_BITFIELD         ( length       , 15 , unsigned );
        SENF_PARSER_FIELD            ( type         , UInt16Parser  );
        SENF_PARSER_PRIVATE_VARIANT  ( destination_ , d_bit_        , 
                                                      (MACAddressParser) (VoidPacketParser) );
        
        SENF_PARSER_FINALIZE( SNDUPacketParser );
        
        MACAddressParser destination()         /// Only defined if d_bit() == \c false
            { return destination_().get<0>(); }

        bool d_bit()                    /// Destination absent bit
            { return d_bit_(); }

        void withDestination()          /// Clear destination absent bit
            { destination_().init<0>(); }
        
        void withoutDestination()       /// Set destination absent bit
            { destination_().init<1>(); }

        UInt32Parser crc() 
            { return parse<UInt32Parser>( data().size() - 4 ); }

        boost::uint32_t calcCrc() const;
    };

    
    struct ULEExtHeaderTypes {
        typedef boost::uint16_t key_t;
    };
    
    /** \brief ULE SNDU Packet
        
        \par Packet type (typedef):
            \ref SNDUPacket

        \par Fields:
            \ref SNDUPacketParser
            \image html SNDUPacket.png

        \ingroup protocolbundle_mpegdvb
     */
    struct SNDUPacketType
        : public PacketTypeBase
//          public PacketTypeMixin<SNDUPacketType, ULEExtHeaderTypes>
    {
//        typedef PacketTypeMixin<SNDUPacketType, ULEExtHeaderType> mixin;
        typedef ConcretePacket<SNDUPacketType> packet;
        typedef SNDUPacketParser parser;

//        using mixin::nextPacketRange;
//        using mixin::nextPacketType;
//        using mixin::init;
        
//        static key_t nextPacketKey(packet p);
        
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
