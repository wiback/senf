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
    \brief DTCPPacket public header */

#ifndef HH_SENF_Packets_MPEGDVBBundle_DTCPPacket_
#define HH_SENF_Packets_MPEGDVBBundle_DTCPPacket_ 1

// Custom includes
#include <senf/Packets/DefaultBundle/IPv4Packet.hh>
#include <senf/Packets/DefaultBundle/IPv6Packet.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

#   define DTCP_V4_MCADDRESS "224.0.0.36"
#   define DTCP_V6_MCADDRESS "FF02:0:0:0:0:0:1:4"
#   define DTCP_UDP_PORT 652

    struct DTCPIPv4AddressListParser : public PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_PRIVATE_FIELD( fbipCount_, UInt8Parser ); //<pkgdraw: hide
        SENF_PARSER_PRIVATE_FIELD( reserved_,  UInt8Parser  ); //<pkgdraw: hide

        SENF_PARSER_VECTOR( fbips, fbipCount_, INet4AddressParser );

        SENF_PARSER_FINALIZE( DTCPIPv4AddressListParser );

        // Needed since we do NOT want to init fbipCount_ or reseverd_. And since
        // INet4AddressParser::init() is a no-op, we can just as well disable init completely
        SENF_PARSER_INIT() {}
    };

    struct DTCPIPv6AddressListParser : public PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_PRIVATE_FIELD( fbipCount_, UInt8Parser ); //<pkgdraw: hide
        SENF_PARSER_PRIVATE_FIELD( reserved_,  UInt8Parser ); //<pkgdraw: hide

        SENF_PARSER_VECTOR( fbips, fbipCount_, INet6AddressParser );

        SENF_PARSER_FINALIZE( DTCPIPv6AddressListParser );

        // Needed since we do NOT want to init fbipCount_ or reseverd_. And since
        // INet4AddressParser::init() is a no-op, we can just as well disable init completely
        SENF_PARSER_INIT() {}
    };

    /** \brief Parse a DTCP HELLO packet

        Parser implementing the DTCP packet according to RFC 3077

        \see DTCPHelloPacketType
     */
    struct DTCPHelloPacketParser : public PacketParserBase
    {
        enum Command { JOIN=1, LEAVE=2 };

#       include SENF_PARSER()

        //>pkgdraw: name=vers
        SENF_PARSER_BITFIELD( versionNumber, 4, unsigned );  // must be 1
        SENF_PARSER_BITFIELD( command,       4, unsigned );  //<pkgdraw: name=cmd

        SENF_PARSER_FIELD( interval,       UInt8Parser  );  // should be 5
        SENF_PARSER_FIELD( sequenceNumber, UInt16Parser );

        SENF_PARSER_PRIVATE_BITFIELD( reserved0_,         3, unsigned );  //<pkgdraw: name=
        SENF_PARSER_BITFIELD        ( receiveCapableFeed, 1, bool     );
        SENF_PARSER_BITFIELD_RO     ( ipVersion,          4, unsigned );  // 4=IPv4, 6=IPv6

        SENF_PARSER_FIELD   ( tunnelProtocol, UInt8Parser );
        SENF_PARSER_FIELD_RO( fbipCount,      UInt8Parser );
        //>pkgdraw: name=
        SENF_PARSER_PRIVATE_FIELD( reserved1_, UInt8Parser );  // must be zero

        // Go back to fbipCount so the variant has access to that field
        SENF_PARSER_GOTO( fbipCount );

        SENF_PARSER_VARIANT( fbipList_, ipVersion,
                ( ids(na, has_v4fbipList, init_v4fbipList, key(4, DTCPIPv4AddressListParser)) )
                ( ids(na, has_v6fbipList, init_v6fbipList, key(6, DTCPIPv6AddressListParser)) ) );

        // We define the two variant accessors ourselves so we can directly return the vector and
        // not the collection parser which contains the vector ...

        typedef DTCPIPv4AddressListParser::fbips_t v4fbipList_t;
        v4fbipList_t v4fbipList() { return fbipList_().get<0>().fbips(); }

        typedef DTCPIPv6AddressListParser::fbips_t v6fbipList_t;
        v6fbipList_t v6fbipList() { return fbipList_().get<1>().fbips(); }

        SENF_PARSER_FINALIZE(DTCPHelloPacketParser);
    };

    /** \brief DTCP HELLO packet

        \par Packet type (typedef):
            \ref DTCPHelloPacket

        \par Fields:
            \ref DTCPHelloPacketParser

        \image html DTCPPacket.png

        \ingroup protocolbundle_mpegdvb
     */
    struct DTCPHelloPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<DTCPHelloPacketType>
    {
        typedef PacketTypeMixin<DTCPHelloPacketType> mixin;
        typedef ConcretePacket<DTCPHelloPacketType> packet;
        typedef DTCPHelloPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;

        static void dump(packet p, std::ostream & os);
    };

    /** \brief DTCP packet typedef */
    typedef DTCPHelloPacketType::packet DTCPHelloPacket;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "DTCPPacket.cci"
//#include "DTCPPacket.ct"
//#include "DTCPPacket.cti"
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
