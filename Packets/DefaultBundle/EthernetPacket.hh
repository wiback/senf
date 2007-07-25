// $id: EthernetPacket.hh 299 2007-07-10 21:23:49Z g0dil $
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
    \brief EthernetPacket public header */

#ifndef HH_EthernetPacket_
#define HH_EthernetPacket_ 1

// Custom includes
#include <algorithm>
#include <boost/array.hpp>
#include "Packets/Packets.hh"

//#include "EthernetPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Ethernet MAC address
        
        The Ethernet MAC is modelled as a fixed-size container/sequence of 6 bytes.

        \todo Move to someplace else when implementing the addressing classes
     */
    struct MACAddress
        : boost::array<PacketParserBase::byte,6>
    {
        MACAddress(std::string addr);
        template <class InputIterator>
        MACAddress(InputIterator i);

        struct SyntaxException : public std::exception
        { virtual char const * what() const throw() { return "invalid mac address syntax"; } };
    };

    /** \brief Parse an Ethernet MAC address 

        The ethernet MAC is returned by value as a 6-byte sequence

        \see MACAddress \n
            EthernetPacket
     */
    struct Parse_MAC : public PacketParserBase
    {
        Parse_MAC(data_iterator i, state_type s) : PacketParserBase(i,s,fixed_bytes) {}
       
        ///////////////////////////////////////////////////////////////////////////

        typedef MACAddress value_type;
        static const size_type fixed_bytes = 6u;

        value_type value() const { return MACAddress(i()); }
        void value(value_type const & v) { std::copy(v.begin(), v.end(), i()); }
        operator value_type () { return value(); }
        byte & operator[](size_type index) { return *boost::next(i(),index);  }

        Parse_MAC const & operator= (value_type const & other) { value(other); return *this; }
    };
    
    /** \brief Parse an Ethernet packet

        Parser implementing an ethernet header.

        \see EthernetPacketType
     */
    struct Parse_Ethernet : public PacketParserBase
    {
        typedef Parse_UInt16                      Parse_Type;

#       ifndef DOXYGEN

        SENF_PACKET_PARSER_INIT(Parse_Ethernet);

        SENF_PACKET_PARSER_DEFINE_FIXED_FIELDS(
            ((Field)( destination, Parse_MAC  ))
            ((Field)( source,      Parse_MAC  ))
            ((Field)( type,        Parse_Type )) );

#       else

        Parse_MAC destination();
        Parse_MAC source();
        Parse_Type type();

#       endif
    };

    /** \brief EtherType registry

        This registry registers packet types with their EtherType number.
        
        \see <a href="http://www.iana.org/assignments/ethernet-numbers">Ethernet numbers</a> \n
            \ref PacketRegistry
     */
    struct EtherTypes {
        // See 
        typedef boost::uint16_t key_t;
    };

    /** \brief Ethernet packet

        \par Packet type (typedef):
            \ref EthernetPacket

        \par Fields:
            \ref Parse_Ethernet

        \par Associated registries:
            \ref EtherTypes

        \ingroup protocolbundle_default
     */
    struct EthernetPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<EthernetPacketType, EtherTypes>
    {
        typedef PacketTypeMixin<EthernetPacketType, EtherTypes> mixin;
        typedef ConcretePacket<EthernetPacketType> packet;
        typedef Parse_Ethernet parser;

        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::initSize;
        using mixin::init;

        /** \todo Add LLC/SNAP support -> only use the registry
            for type() values >=1536, otherwise expect an LLC header */
        static registry_key_t nextPacketKey(packet p) 
            { return p->type(); }

        static void dump(packet p, std::ostream & os);
    };

    /** \brief Ethernet packet typedef */
    typedef EthernetPacketType::packet EthernetPacket;

    /** \brief Parse an ethernet VLAN tag
        
        Parser interpreting the ethernet VLAN tag. Fields are

        \see EthVLanPacketType
     */
    struct Parse_EthVLan : public PacketParserBase
    {
        typedef Parse_UIntField < 0,  3 > Parse_Priority;
        typedef Parse_Flag          < 3 > Parse_CFI;
        typedef Parse_UIntField < 4, 16 > Parse_VLanId;
        typedef Parse_UInt16              Parse_Type;

#       ifndef DOXYGEN

        SENF_PACKET_PARSER_INIT(Parse_EthVLan);

        SENF_PACKET_PARSER_DEFINE_FIXED_FIELDS(
            ((OverlayField)( priority, Parse_Priority ))
            ((OverlayField)( cfi,      Parse_CFI      ))
            ((Field       )( vlanId,   Parse_VLanId   ))
            ((Field       )( type,     Parse_Type     )) );

#       else

        Parse_Priority priority();
        Parse_CFI cfi();
        Parse_VLanId vlanId();
        Parse_Type type();

#       endif
    };

    /** \brief Ethernet VLAN tag

        \par Packet type (typedef):
            \ref EthVLanPacket

        \par Fields:
            \ref Parse_EthVLan

        \par Associated registries:
            \ref EtherTypes

        \ingroup protocolbundle_default
     */
    struct EthVLanPacketType
        : public PacketTypeBase, 
          public PacketTypeMixin<EthVLanPacketType, EtherTypes>
    {
        typedef PacketTypeMixin<EthVLanPacketType, EtherTypes> mixin;
        typedef ConcretePacket<EthVLanPacketType> packet;
        typedef Parse_EthVLan parser;

        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::initSize;
        using mixin::init;

        /** \todo Add LLC/SNAP support -> only use the registry
            for type() values >=1536, otherwise expect an LLC header */
        static registry_key_t nextPacketKey(packet p) 
            { return p->type(); }

        static void dump(packet p, std::ostream & os);
    };

    /** \brief Ethernet VLAN tag typedef */
    typedef EthVLanPacketType::packet EthVLanPacket;
}


///////////////////////////////hh.e////////////////////////////////////////
#endif
#ifndef SENF_PACKETS_DECL_ONLY
//#include "EthernetPacket.cci"
#include "EthernetPacket.ct"
//#include "EthernetPacket.cti"
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
