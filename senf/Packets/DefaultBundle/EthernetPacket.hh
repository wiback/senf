//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief EthernetPacket public header */

#ifndef HH_SENF_Packets_DefaultBundle_EthernetPacket_
#define HH_SENF_Packets_DefaultBundle_EthernetPacket_ 1

// Custom includes
#include <senf/Socket/Protocols/Raw/MACAddress.hh>
#include <senf/Packets/Packets.hh>
#include "Registries.hh"

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Parse an Ethernet MAC address

        The ethernet MAC is returned by value as a 6-byte sequence

        \see MACAddress \n
            EthernetPacket
     */
    struct MACAddressParser
        : public ArrayValueParserBase<MACAddressParser, MACAddress>
    {
        MACAddressParser(data_iterator i, state_type s) : Base(i,s) {}

        bool local() const;             ///< \c true, if address is locally administered
        bool multicast() const;         ///< \c true, if address is a group/multicast address
        bool broadcast() const;         ///< \c true, if address is the broadcast address
        bool null() const;              ///< \c true, if address is the zero address
        void hash(boost::uint64_t *, boost::uint16_t const & otherKey = 0) const;   ///< Return MAC address as uint64 value as hask key

        using Base::operator=;
    };


    /** \brief Parse an Ethernet packet

        Parser implementing an ethernet header.

        \see EthernetPacketType
     */
    struct EthernetPacketParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD( destination, MACAddressParser );
        SENF_PARSER_FIELD( source,      MACAddressParser );
        SENF_PARSER_FIELD( type_length, UInt16Parser     );

        SENF_PARSER_FINALIZE( EthernetPacketParser );
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
        typedef PacketTypeMixin<EthernetPacketType, EtherTypes> mixin;
        typedef ConcretePacket<EthernetPacketType> packet;
        typedef EthernetPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::initSize;
        using mixin::init;

        static factory_t nextPacketType(packet p);
        /// Dump given EthernetPacket in readable form to given output stream
        static void dump(packet p, std::ostream & os);
        static void finalize(packet p);

        static const unsigned minPayloadSize    = 60u;  // without CRC - that's what we typically 'see' in SENF
        static const unsigned minPayloadSizeCRC = 64u;  // real h/w limit on 100Mbps Ethernet
    };

    /** \brief Ethernet packet typedef
        \ingroup protocolbundle_default
     */
    typedef ConcretePacket<EthernetPacketType> EthernetPacket;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( EthernetPacket );


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

        // field is named "type_length" analogue to EthernetPacketParser
        SENF_PARSER_FIELD( type_length, UInt16Parser );

        SENF_PARSER_FINALIZE( EthVLanPacketParser );
    };

    /** \brief Ethernet VLAN S-Tag

        \par Packet type (typedef):
            \ref EthVLanSPacket

        \par Fields:
            \ref EthVLanPacketParser
            \image html EthVLanPacket.png

        \par Associated registries:
            \ref EtherTypes

        \par Finalize action:
            Set \a type from type of next packet if found in \ref EtherTypes

        \ingroup protocolbundle_default
     */
    struct EthVLanSPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<EthVLanSPacketType, EtherTypes>
    {
        typedef PacketTypeMixin<EthVLanSPacketType, EtherTypes> mixin;
        typedef ConcretePacket<EthVLanSPacketType> packet;
        typedef EthVLanPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::initSize;
        using mixin::init;

        /** \todo Add LLC/SNAP support -> only use the registry
            for type() values >=1536, otherwise expect an LLC header */
        static key_t nextPacketKey(packet p)
            { return p->type_length(); }

        /// Dump given EthVLanPacket in readable form to given output stream
        static void dump(packet p, std::ostream & os);
        static void finalize(packet p);

        // S-TAG
        static const EtherTypes::key_t etherType = 0x88a8;
    };

    /** \brief Ethernet VLAN S-Tag typedef
        \ingroup protocolbundle_default
     */
    typedef ConcretePacket<EthVLanSPacketType> EthVLanSPacket;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( EthVLanSPacket );

    /** \brief Ethernet VLAN C-Tag

        \par Packet type (typedef):
            \ref EthVLanCPacket

        \par Fields:
            \ref EthVLanPacketParser
            \image html EthVLanPacket.png

        \par Associated registries:
            \ref EtherTypes

        \par Finalize action:
            Set \a type from type of next packet if found in \ref EtherTypes

        \ingroup protocolbundle_default
     */
    struct EthVLanCPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<EthVLanCPacketType, EtherTypes>
    {
        typedef PacketTypeMixin<EthVLanCPacketType, EtherTypes> mixin;
        typedef ConcretePacket<EthVLanCPacketType> packet;
        typedef EthVLanPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::initSize;
        using mixin::init;

        /** \todo Add LLC/SNAP support -> only use the registry
            for type() values >=1536, otherwise expect an LLC header */
        static key_t nextPacketKey(packet p)
            { return p->type_length(); }

        /// Dump given EthVLanPacket in readable form to given output stream
        static void dump(packet p, std::ostream & os);
        static void finalize(packet p);

        // C-TAG
        static const EtherTypes::key_t etherType = 0x8100;
    };

    /** \brief Ethernet VLAN C-Tag typedef
        \ingroup protocolbundle_default
     */
    typedef ConcretePacket<EthVLanCPacketType> EthVLanCPacket;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( EthVLanCPacket );


    
    struct EthOUIExtensionPacketParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD( oui,      UInt24Parser );
        SENF_PARSER_FIELD( ext_type, UInt16Parser );

        SENF_PARSER_FINALIZE( EthOUIExtensionPacketParser );
    };

    struct EthOUIExtensionPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<EthOUIExtensionPacketType, EtherOUIExtTypes>
    {
        typedef PacketTypeMixin<EthOUIExtensionPacketType, EtherOUIExtTypes> mixin;
        typedef ConcretePacket<EthOUIExtensionPacketType> packet;
        typedef EthOUIExtensionPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::initSize;
        using mixin::init;

        static key_t nextPacketKey(packet p)
            { return EtherOUIExtTypes::type(p->oui(), p->ext_type()); }
        static void dump(packet p, std::ostream & os);
        static void finalize(packet p);

        static const EtherTypes::key_t etherType = 0x88b7;

        static const std::uint32_t OUI_Fraunhofer_FOKUS = 0x001113;
        static const std::uint32_t OUI_Fraunhofer_FIT   = 0x3c11b2;
    };

    typedef ConcretePacket<EthOUIExtensionPacketType> EthOUIExtensionPacket;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( EthOUIExtensionPacket );

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "EthernetPacket.cci"
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
