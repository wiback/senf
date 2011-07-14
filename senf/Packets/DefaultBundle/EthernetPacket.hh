// $Id$
//
// Copyright (C) 2006
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
    \brief EthernetPacket public header */

#ifndef HH_SENF_Packets_DefaultBundle_EthernetPacket_
#define HH_SENF_Packets_DefaultBundle_EthernetPacket_ 1

// Custom includes
#include <senf/Socket/Protocols/Raw/MACAddress.hh>
#include <senf/Packets/Packets.hh>

//#include "EthernetPacket.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Parse an Ethernet MAC address

        The ethernet MAC is returned by value as a 6-byte sequence

        \see MACAddress \n
            EthernetPacket
     */
    struct MACAddressParser
        : public ValueParserBase<MACAddressParser, MACAddress, 6u>
    {
        MACAddressParser(data_iterator i, state_type s) : Base(i,s) {}

        value_type value() const { return MACAddress::from_data(i()); }
        void value(value_type const & v) { std::copy(v.begin(), v.end(), i()); }

        using Base::operator=;
    };

    /** \brief Parse an Ethernet packet

        Parser implementing an ethernet header.

        \see EthernetPacketType
     */
    struct EthernetPacketParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD( destination, MACAddressParser    );
        SENF_PARSER_FIELD( source,      MACAddressParser    );
        SENF_PARSER_FIELD( type_length, UInt16Parser );

        SENF_PARSER_FINALIZE(EthernetPacketParser);
    };

    /** \brief EtherType registry

        This registry registers packet types with their EtherType number.

        \see <a href="http://www.iana.org/assignments/ethernet-numbers">Ethernet numbers</a> \n
            \ref PacketRegistry
     */
    struct EtherTypes {
        typedef boost::uint16_t key_t;
    };

    /** \brief Ethernet packet

        \par Packet type (typedef):
            \ref EthernetPacket

        \par Fields:
            \ref EthernetPacketParser
            \image html EthernetPacket.png

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
#endif
        typedef ConcretePacket<EthernetPacketType> packet;
        typedef EthernetPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::initSize;
        using mixin::init;

        static factory_t nextPacketType(packet p);
        /// Dump given EthernetPacket in readable form to given output stream
        static void dump(packet p, std::ostream & os);
        static void finalize(packet p);
    };

    /** \brief Ethernet packet typedef
        \ingroup protocolbundle_default
     */
    typedef ConcretePacket<EthernetPacketType> EthernetPacket;


    /** \brief Parse an ethernet VLAN tag

        Parser interpreting the ethernet VLAN tag. Fields are

        \see EthVLanPacketType
     */
    struct EthVLanPacketParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_BITFIELD( priority,  3, unsigned );
        SENF_PARSER_BITFIELD( cfi,       1, bool     );
        SENF_PARSER_BITFIELD( vlanId,   12, unsigned );

        SENF_PARSER_FIELD( type, UInt16Parser );

        SENF_PARSER_FINALIZE(EthVLanPacketParser);
    };

    /** \brief Ethernet VLAN tag

        \par Packet type (typedef):
            \ref EthVLanPacket

        \par Fields:
            \ref EthVLanPacketParser
            \image html EthVLanPacket.png

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
#endif
        typedef ConcretePacket<EthVLanPacketType> packet;
        typedef EthVLanPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::initSize;
        using mixin::init;

        /** \todo Add LLC/SNAP support -> only use the registry
            for type() values >=1536, otherwise expect an LLC header */
        static key_t nextPacketKey(packet p)
            { return p->type(); }

        /// Dump given EthVLanPacket in readable form to given output stream
        static void dump(packet p, std::ostream & os);
        static void finalize(packet p);
    };

    /** \brief Ethernet VLAN tag typedef
        \ingroup protocolbundle_default
     */
    typedef ConcretePacket<EthVLanPacketType> EthVLanPacket;

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
