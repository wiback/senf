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
    \brief IPv6Packet public header */

#ifndef HH_SENF_Packets_DefaultBundle_IPv6Packet_
#define HH_SENF_Packets_DefaultBundle_IPv6Packet_ 1

// Custom includes
#include <senf/Socket/Protocols/INet/INet6Address.hh>
#include <senf/Packets/Packets.hh>
#include "Registries.hh"

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Parse an IPv6 address

        \see INet6Address
     */
    struct INet6AddressParser
        : public ArrayValueParserBase<INet6AddressParser, INet6Address>
    {
        INet6AddressParser(data_iterator i, state_type s) : Base(i,s) {}

        using Base::operator=;
    };

    /** \brief Parse an IPv6 packet

        \image html IPv6Packet.png
        \see IPv6PacketType \n
            <a href="http://tools.ietf.org/html/rfc2460">RFC 2460</a>
     */
    struct IPv6PacketParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_BITFIELD( version,       4, unsigned );
        SENF_PARSER_BITFIELD( trafficClass,  8, unsigned );
        SENF_PARSER_BITFIELD( flowLabel,    20, unsigned );

        SENF_PARSER_FIELD( length,       UInt16Parser       );
        SENF_PARSER_FIELD( nextHeader,   UInt8Parser        );
        SENF_PARSER_FIELD( hopLimit,     UInt8Parser        );
        SENF_PARSER_FIELD( source,       INet6AddressParser );
        SENF_PARSER_FIELD( destination,  INet6AddressParser );

        SENF_PARSER_FINALIZE( IPv6PacketParser );

        SENF_PARSER_INIT() {
            version() = 6;
        }
    };

    /** \brief IPv6 packet

        \par Packet type (typedef):
            \ref IPv6Packet

        \par Fields:
            \ref IPv6PacketParser

        <table class="packet" cellpadding="5" cellspacing="1" border="1">
          <tr>
            <th width="12.5%">0</th>  <th width="12.5%">4</th> <th width="12.5%">8</th>
            <th width="12.5%">12</th> <th width="12.5%">16</th> <th width="12.5%">20</th>
            <th width="12.5%">24</th> <th width="6.5%">28</th>
            <th style="text-align:right" width="6%">31</th>
          </tr><tr>
            <td>\ref IPv6PacketParser::version() "Version"</td>
            <td colspan="2">\ref IPv6PacketParser::trafficClass() "Traffic Class"</td>
            <td colspan="6">\ref IPv6PacketParser::flowLabel() "Flow Label"</td>
          </tr><tr>
            <td colspan="4">\ref IPv6PacketParser::length() "Payload Length"</td>
            <td colspan="2">\ref IPv6PacketParser::nextHeader() "Next Header"</td>
            <td colspan="3">\ref IPv6PacketParser::hopLimit() "Hop Limit"</td>
          </tr><tr>
            <td colspan="9" style="height:8em">
              \ref IPv6PacketParser::source() "Source Address"</td>
          </tr><tr>
            <td colspan="9" style="height:8em">
              \ref IPv6PacketParser::destination()  "Destination Address"</td>
          </tr>
        </table>

        \par Associated registries:
            \ref IPTypes

        \par Finalize action:
            \copydetails finalize()

        \ingroup protocolbundle_default
     */
    struct IPv6PacketType
        : public PacketTypeBase,
          public PacketTypeMixin<IPv6PacketType, IPTypes>
    {
        typedef PacketTypeMixin<IPv6PacketType, IPTypes> mixin;
        typedef ConcretePacket<IPv6PacketType> packet; ///< IPv6 packet typedef
        typedef IPv6PacketParser parser;               ///< typedef to the parser of IPv6 packet

        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::initSize;
        using mixin::init;

        static key_t nextPacketKey(packet p)
            { return p->nextHeader(); }

        /** \brief Dump given IPv6Packet in readable form to given output stream */
        static void dump(packet p, std::ostream & os);

        static void finalize(packet p); ///< Finalize packet.
                                        /**< \li set \ref IPv6PacketParser::length() "length"
                                               from payload size
                                             \li set \ref IPv6PacketParser::nextHeader()
                                               "nextHeader" from type of next packet if found
                                               in \ref IPTypes */

        static const EtherTypes::key_t etherType = 0x86dd;
    };

    /** \brief IPv6 packet typedef
        \ingroup protocolbundle_default
     */
    typedef ConcretePacket<IPv6PacketType> IPv6Packet;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( IPv6Packet );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "IPv6Packet.cci"
//#include "IPv6Packet.ct"
//#include "IPv6Packet.cti"
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
