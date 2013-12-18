// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief SNDUPacket public header */

#ifndef HH_SENF_Packets_MPEGDVBBundle_SNDUPacket_
#define HH_SENF_Packets_MPEGDVBBundle_SNDUPacket_ 1

// Custom includes
#include <boost/crc.hpp>
#include <senf/Packets/Packets.hh>
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>

//#include "SNDUPacket.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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

        SENF_PARSER_PRIVATE_BITFIELD( d_bit_,  1 , unsigned );
        SENF_PARSER_BITFIELD        ( length, 15 , unsigned );
        SENF_PARSER_FIELD           ( type,   UInt16Parser  );
        SENF_PARSER_PRIVATE_VARIANT ( destination_, d_bit_,
                (MACAddressParser) (VoidPacketParser) );

        SENF_PARSER_FINALIZE( SNDUPacketParser );

        MACAddressParser destination()  ///< Only defined if d_bit() == \c false
            { return destination_().get<0>(); }

        bool d_bit()                    ///< Destination absent bit
            { return d_bit_(); }

        void withDestination()          ///< Clear destination absent bit
            { destination_().init<0>(); }

        void withoutDestination()       ///< Set destination absent bit
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


//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
