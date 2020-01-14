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
    \brief Packets public header */

#ifndef HH_SENF_Ext_NetEmu_Packets_
#define HH_SENF_Ext_NetEmu_Packets_ 1

// Custom includes
#include <senf/Packets/Packets.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    /** \brief Emulated network packet header parser
         \see PacketHeaderType
         \ingroup senf_emu_emulation
      */
     struct PacketHeaderParser : public PacketParserBase
     {
#       include SENF_FIXED_PARSER()

         SENF_PARSER_FIELD( interfaceType,   UInt32Parser );
         SENF_PARSER_FIELD( sequenceNumber,  UInt32Parser );
         SENF_PARSER_FIELD( timestamp,       Int64Parser  );
         SENF_PARSER_FIELD( nodeId,          UInt32Parser );
         SENF_PARSER_FIELD( interfaceIndex,  UInt32Parser );

         SENF_PARSER_FINALIZE(PacketHeaderParser);
     };

     /** \brief Emulated interface header type packet registry
         \ingroup senf_emu_emulation
      */
     struct InterfaceTypes
     {
         typedef boost::uint32_t key_t;
     };

     /** \brief Emulated interface packet header

         Emulated interfaces send their packets to other interfaces in the same collision domain via
         multicast sockets. Additional meta information about the packet is contained in this packet
         header. The complete packet chain of an emulated packet on the wire is:
         \li PacketHeader: This header holds generic emulation parameters and the interface type.
         \li <em>Interface specific packet header</em>: Here, each interface type adds an additional
             header with optional additional information
         \li <em>Payload</em>

         \ingroup senf_emu_emulation
      */
     struct PacketHeaderType
         : public PacketTypeBase,
           public PacketTypeMixin<PacketHeaderType, InterfaceTypes>
     {
         typedef PacketTypeMixin<PacketHeaderType, InterfaceTypes> mixin;
         typedef ConcretePacket<PacketHeaderType> packet; ///< PacketHeader packet typedef
         typedef PacketHeaderParser parser;               ///< typedef to the parser of PacketHeader

         using mixin::nextPacketRange;
         using mixin::nextPacketType;
         using mixin::initSize;
         using mixin::init;

         static key_t nextPacketKey(packet p);
         /// Dump given PacketHeader in readable form to given output stream
         static void dump(packet p, std::ostream & os);
         static void finalize(packet p);
     };

     /** \brief PacketHeader packet typedef */
     typedef ConcretePacket<PacketHeaderType> PacketHeader;


     /** \brief Helper to log all emulated packet information

         This helper structure is used to log all information on an emulated network packet for
         debugging purposes

         \code
         Packet p (...);
         SENF_LOG(( "Got new packet:\n" << dumpPacketInfo(p) ));
         \endcode

         \ingroup senf_emu_emulation
      */
     struct dumpPacketInfo
     {
         dumpPacketInfo(senf::Packet const & p) : packet (p) {}
         Packet packet;
     };

     /** \related dumpPacketInfo */
     std::ostream & operator<<(std::ostream & os, dumpPacketInfo const & info);


    /** \brief Internal: WLAN Specific emulated interface packet header
        \see WLANPacketHeaderType */
    struct WLANPacketHeaderParser : public PacketParserBase
    {
#       include SENF_PARSER()
        SENF_PARSER_BITFIELD( mgt, 1,  bool         );
        SENF_PARSER_FINALIZE( WLANPacketHeaderParser);
        SENF_PARSER_INIT() {
            mgt() = false;
        }
    };

    /** \brief Internal WLAN Specific emulated interface packet type
        \see WLANPacketHeaderParser */
    struct WLANPacketHeaderType
        : public PacketTypeBase,
          public PacketTypeMixin<WLANPacketHeaderType>
    {
        typedef PacketTypeMixin<WLANPacketHeaderType> mixin;
        typedef ConcretePacket<WLANPacketHeaderType> packet;
        typedef WLANPacketHeaderParser parser;

        using mixin::nextPacketRange;
        using mixin::initSize;
        using mixin::init;

        static factory_t nextPacketType(packet p);
        static void dump(packet p, std::ostream & os);
        static void finalize(packet p);
    };

    typedef ConcretePacket<WLANPacketHeaderType> WLANPacketHeader;


    /** \brief Internal: Wireless Specific emulated interface packet header
        \see EmulatedWirelessPacketHeaderType */
    struct EmulatedWirelessPacketHeaderParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD( interfaceType,   UInt32Parser );
        SENF_PARSER_FIELD( txPower,         Int16Parser  );
        SENF_PARSER_FIELD( frequency,       UInt32Parser );
        SENF_PARSER_FIELD( bandwidth,       UInt16Parser );
        SENF_PARSER_FIELD( modulationId,    UInt32Parser );

        SENF_PARSER_FINALIZE(EmulatedWirelessPacketHeaderParser);
    };

    /** \brief Internal Wireless Specific emulated interface packet type
        \see EmulatedWirelessPacketHeaderParser */
    struct EmulatedWirelessPacketHeaderType
        : public PacketTypeBase,
          public PacketTypeMixin<EmulatedWirelessPacketHeaderType, InterfaceTypes>
    {
        typedef PacketTypeMixin<EmulatedWirelessPacketHeaderType, InterfaceTypes> mixin;
        typedef ConcretePacket<EmulatedWirelessPacketHeaderType> packet;
        typedef EmulatedWirelessPacketHeaderParser parser;

        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::initSize;
        using mixin::init;

        static key_t nextPacketKey(packet p);
        static void dump(packet p, std::ostream & os);
        static void finalize(packet p);
    };

    typedef ConcretePacket<EmulatedWirelessPacketHeaderType> EmulatedWirelessPacketHeader;


    /** \brief Internal: DVB Specific emulated interface packet header
        \see DVBPacketHeaderType */
    struct DVBPacketHeaderParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FINALIZE(DVBPacketHeaderParser);
    };

    /** \brief Internal DVB Specific emulated interface packet type
        \see DVBPacketHeaderParser */
    struct DVBPacketHeaderType
        : public PacketTypeBase,
          public PacketTypeMixin<DVBPacketHeaderType>
    {
        typedef PacketTypeMixin<DVBPacketHeaderType> mixin;
        typedef ConcretePacket<DVBPacketHeaderType> packet;
        typedef DVBPacketHeaderParser parser;

        using mixin::nextPacketRange;
        using mixin::initSize;
        using mixin::init;

        static factory_t nextPacketType(packet p);
        static void dump(packet p, std::ostream & os);
    };

    typedef ConcretePacket<DVBPacketHeaderType> DVBPacketHeader;


   /** \brief Internal: Ethernet Specific emulated interface packet header
        \see EthernetPacketHeaderType */
    struct EthernetPacketHeaderParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FINALIZE(EthernetPacketHeaderParser );
    };

    /** \brief Internal DVB Specific emulated interface packet type
        \see DVBPacketHeaderParser */
    struct EthernetPacketHeaderType
        : public PacketTypeBase,
          public PacketTypeMixin<EthernetPacketHeaderType>
    {
        typedef PacketTypeMixin<EthernetPacketHeaderType> mixin;
        typedef ConcretePacket<EthernetPacketHeaderType> packet;
        typedef EthernetPacketHeaderParser parser;

        using mixin::nextPacketRange;
        using mixin::initSize;
        using mixin::init;

        static factory_t nextPacketType(packet p);
        static void dump(packet p, std::ostream & os);
    };

    typedef ConcretePacket<EthernetPacketHeaderType> EthernetPacketHeader;
}};

SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( senf::emu::PacketHeader                 );
SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( senf::emu::WLANPacketHeader             );
SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( senf::emu::EmulatedWirelessPacketHeader );
SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( senf::emu::DVBPacketHeader              );
SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( senf::emu::EthernetPacketHeader         );


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "Packets.cci"
//#include "Packets.ct"
//#include "Packets.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
