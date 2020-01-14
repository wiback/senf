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
    \brief GREPacket public header */

#ifndef HH_SENF_Packets_MPEGDVBBundle_GREPacket_
#define HH_SENF_Packets_MPEGDVBBundle_GREPacket_ 1

// Custom includes
#include <senf/Packets/Packets.hh>
#include <senf/Packets/DefaultBundle/Registries.hh>

//#include "GREPacket.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Parse a GRE packet

        Parser implementing the header of a General Routing Encapsulation (GRE, RFC 2784) Packet

        \see GREPacketType
     */
    struct GREChecksumParser : public PacketParserBase {
#       include SENF_PARSER()
        SENF_PARSER_FIELD        ( checksum1_, UInt16Parser );
        SENF_PARSER_PRIVATE_FIELD( reserved1_, UInt16Parser );
        SENF_PARSER_FINALIZE( GREChecksumParser );
    };

    struct GREPacketParser : public PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_BITFIELD        ( checksum_present,  1, bool     );
        SENF_PARSER_PRIVATE_BITFIELD( reserved0_,       12, unsigned ); // TODO: SKIP !!
        SENF_PARSER_BITFIELD        ( version_number,    3, unsigned ); // TODO: Always Zero !!
        SENF_PARSER_FIELD           ( protocol_type,    UInt16Parser );
        SENF_PARSER_PRIVATE_VARIANT ( checksum_,  checksum_present,
                (VoidPacketParser) (GREChecksumParser) );

        SENF_PARSER_FINALIZE( GREPacketParser );

      private:
        UInt16Parser checksum() const /// only defined if checksum_present() == \c true
             { return checksum_().get<1>().checksum1_(); }
    };

    /** \brief GRE packet

        \par Packet type (typedef):
            \ref GREPacket

        \par Fields:
            \ref GREPacketParser
            \image html GREPacket.png

        \ingroup protocolbundle_mpegdvb
     */
    struct GREPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<GREPacketType, EtherTypes>
    {
        typedef PacketTypeMixin<GREPacketType, EtherTypes> mixin;
        typedef ConcretePacket<GREPacketType> packet;
        typedef GREPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::init;
        using mixin::initSize;

        static void dump(packet p, std::ostream & os);
        static EtherTypes::key_t nextPacketKey(packet p) {
            return p->protocol_type();
        }
        static void finalize(packet p);

        static const IPTypes::key_t ipType = 47;
    };

    /** \brief GRE packet typedef */
    typedef GREPacketType::packet GREPacket;


}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "TransportPacket.cci"
//#include "TransportPacket.ct"
//#include "TransportPacket.cti"
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
