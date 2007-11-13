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
#include "../../Socket/Protocols/Raw/MACAddress.hh"
#include "../../Packets/Packets.hh"

//#include "EthernetPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

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

        value_type value() const { return MACAddress::from_data(i()); }
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
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD( destination, Parse_MAC    );
        SENF_PARSER_FIELD( source,      Parse_MAC    );
        SENF_PARSER_FIELD( type,        Parse_UInt16 );

        SENF_PARSER_FINALIZE(Parse_Ethernet);
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

        \par Finalize action:
            Set \a type from type of next packet if found in \ref EtherTypes

        \ingroup protocolbundle_default
     */
    struct EthernetPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<EthernetPacketType, EtherTypes>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<EthernetPacketType, EtherTypes> mixin;
        typedef ConcretePacket<EthernetPacketType> packet;
        typedef Parse_Ethernet parser;
#endif
        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::initSize;
        using mixin::init;

        /** \todo Add LLC/SNAP support -> only use the registry
            for type() values >=1536, otherwise expect an LLC header */
        static registry_key_t nextPacketKey(packet p) 
            { return p->type(); }

        static void dump(packet p, std::ostream & os);
        static void finalize(packet p);
    };

    /** \brief Ethernet packet typedef */
    typedef ConcretePacket<EthernetPacketType> EthernetPacket;

    /** \brief Parse an ethernet VLAN tag
        
        Parser interpreting the ethernet VLAN tag. Fields are

        \see EthVLanPacketType
     */
    struct Parse_EthVLan : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_BITFIELD( priority,  3, unsigned );
        SENF_PARSER_BITFIELD( cfi,       1, bool     );
        SENF_PARSER_BITFIELD( vlanId,   12, unsigned );

        SENF_PARSER_FIELD( type, Parse_UInt16 );

        SENF_PARSER_FINALIZE(Parse_EthVLan);
    };

    /** \brief Ethernet VLAN tag

        \par Packet type (typedef):
            \ref EthVLanPacket

        \par Fields:
            \ref Parse_EthVLan

        \par Associated registries:
            \ref EtherTypes

        \par Finalize action:
            Set \a type from type of next packet if found in \ref EtherTypes

        \ingroup protocolbundle_default
     */
    struct EthVLanPacketType
        : public PacketTypeBase, 
          public PacketTypeMixin<EthVLanPacketType, EtherTypes>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<EthVLanPacketType, EtherTypes> mixin;
        typedef ConcretePacket<EthVLanPacketType> packet;
        typedef Parse_EthVLan parser;
#endif
        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::initSize;
        using mixin::init;

        /** \todo Add LLC/SNAP support -> only use the registry
            for type() values >=1536, otherwise expect an LLC header */
        static registry_key_t nextPacketKey(packet p) 
            { return p->type(); }

        static void dump(packet p, std::ostream & os);
        static void finalize(packet p);
    };

    /** \brief Ethernet VLAN tag typedef */
    typedef ConcretePacket<EthVLanPacketType> EthVLanPacket;
}


///////////////////////////////hh.e////////////////////////////////////////
#endif
#ifndef SENF_PACKETS_DECL_ONLY
//#include "EthernetPacket.cci"
//#include "EthernetPacket.ct"
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
