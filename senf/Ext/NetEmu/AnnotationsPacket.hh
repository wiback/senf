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
    \brief AnnotationsPacket public header */

#ifndef HH_SENF_Ext_NetEmu_AnnotationsPacket_
#define HH_SENF_Ext_NetEmu_AnnotationsPacket_ 1

// Custom includes
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
    /** \brief Parse an Annotations packet

        Parser implementing an ethernet header.

        \see AnnotationsPacketType
     */
    struct AnnotationsPacketParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD   ( interfaceId,  MACAddressParser   );
        SENF_PARSER_FIELD   ( timestampMAC, senf::UInt64Parser );
        SENF_PARSER_FIELD   ( timestamp,    senf::Int64Parser  );
        SENF_PARSER_FIELD   ( modulationId, senf::UInt32Parser );
        SENF_PARSER_FIELD   ( snr,          senf::UInt16Parser );
        SENF_PARSER_FIELD   ( rssi,         senf::Int16Parser  );
        SENF_PARSER_FIELD   ( airTime,      senf::UInt16Parser );
        SENF_PARSER_FIELD   ( type,         senf::UInt8Parser  );

        SENF_PARSER_BITFIELD( corrupt,       1, bool           );
        SENF_PARSER_BITFIELD( retransmitted, 1, bool           );
        SENF_PARSER_BITFIELD( duplicated,    1, bool           );
        SENF_PARSER_BITFIELD( reordered,     1, bool           ); 
        SENF_PARSER_BITFIELD( aggregated,    1, bool           );
        SENF_PARSER_BITFIELD( unused,        7, unsigned       );
        SENF_PARSER_BITFIELD( gap,           8, unsigned       );
        SENF_PARSER_BITFIELD( length,       12, unsigned       );

        SENF_PARSER_FINALIZE( AnnotationsPacketParser );
    };

    /** \brief Annotations packet

        \par Packet type (typedef):
            \ref AnnotationsPacket

        \par Fields:
            \ref AnnotationsPacketParser
            \image html AnnotationsPacket.png

        \par Associated registries:
            \ref EtherTypes

        \par Finalize action:
            Set \a type from type of next packet if found in \ref EtherTypes

        \ingroup protocolbundle_default
     */
    struct AnnotationsPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<AnnotationsPacketType>
    {
        typedef PacketTypeMixin<AnnotationsPacketType> mixin;
        typedef ConcretePacket<AnnotationsPacketType> packet;
        typedef AnnotationsPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::initSize;
        using mixin::init;

        static factory_t nextPacketType(packet p);
        /// Dump given AnnotationsPacket in readable form to given output stream
        static void dump(packet p, std::ostream & os);
        static void finalize(packet p);

        static const std::uint16_t extType = 0x50;
    };

    /** \brief Annotations packet typedef
        \ingroup protocolbundle_default
     */
    typedef ConcretePacket<AnnotationsPacketType> AnnotationsPacket;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( AnnotationsPacket );

    EthernetPacket prependAnnotationsPacket(Packet const & pkt, MACAddress const & src_ = senf::MACAddress::None, MACAddress const & dst_ = senf::MACAddress::Broadcast);
}
//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "AnnotationsPacket.cci"
//#include "AnnotationsPacket.ct"
//#include "AnnotationsPacket.cti"
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
