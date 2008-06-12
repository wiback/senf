// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     David Wagner <dw6@berlios.de>
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


#ifndef DTCPPACKET_HH_
#define DTCPPACKET_HH_

#include "../../Packets/Packets.hh"
#include "../../Packets/DefaultBundle/IPv4Packet.hh"
#include "../../Packets/DefaultBundle/IPv6Packet.hh"

#define DTCP_V4_MCADDRESS "224.0.0.36"
#define DTCP_V6_MCADDRESS "FF02:0:0:0:0:0:1:4"
#define DTCP_UDP_PORT 652

namespace senf {
    
    //first we have to define some helpers
    struct DTCPIPv4AddressListParser : public PacketParserBase {
#       include SENF_PARSER()        
        SENF_PARSER_PRIVATE_FIELD ( num_of_fbips, UInt8Parser );
        SENF_PARSER_PRIVATE_FIELD ( reserved ,    UInt8Parser );   //must be zero 
        SENF_PARSER_VECTOR        ( fbiplist,     num_of_fbips, INet4AddressParser );

        SENF_PARSER_FINALIZE(DTCPIPv4AddressListParser);
    };
        
    struct DTCPIPv6AddressListParser : public PacketParserBase {
#       include SENF_PARSER()        
        SENF_PARSER_PRIVATE_FIELD ( num_of_fbips, UInt8Parser );
        SENF_PARSER_PRIVATE_FIELD ( reserved,     UInt8Parser );   //must be zero 
        SENF_PARSER_VECTOR        ( fbiplist,     num_of_fbips, INet6AddressParser );

        SENF_PARSER_FINALIZE(DTCPIPv6AddressListParser);
    };

    /** \brief Parse a DTCP packet

        Parser implementing the DTCP packet according to RFC 3077
        
        \see DTCPPacketType
     */
    struct DTCPPacketParser : public PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_BITFIELD         ( version_number,       4, unsigned );  // =1 according to rfc3077
        SENF_PARSER_BITFIELD         ( command,              4, unsigned );  // 1=JOIN 2=LEAVE
        SENF_PARSER_FIELD            ( interval,             UInt8Parser );  // 5 according to rfc3077
        SENF_PARSER_FIELD            ( sequence_number,      UInt16Parser );
        SENF_PARSER_PRIVATE_BITFIELD ( reserved,             3, unsigned );
        SENF_PARSER_BITFIELD         ( receive_capable_feed, 1, bool );      // 0=send only, 1=receive_capable_feed
        SENF_PARSER_BITFIELD_RO      ( ip_version,           4, unsigned );  // 4=IPv4, 6=IPv6
        SENF_PARSER_FIELD    ( tunnel_protocol,      UInt8Parser ); 
        /* Please consider the following comments on the implementation given in this class: 
         * 1. you could think of simply using SENF_PARSER_PRIVATE_VARIANT and List / Vectorparser like this:
         * SENF_PARSER_PRIVATE_VARIANT  ( fbiplist,             ip_version,
         *                                                       (senf::VoidPacketParser) //ip_version=0
         *                                                       (senf::VoidPacketParser) //1
         *                                                       (senf::VoidPacketParser) //2
         *                                                       (senf::VoidPacketParser) //3
         *                                                       (senf::ListBParser< IPv4Packet, num_of_fbips>) //4 
         *                                                       (senf::VoidPacketParser) //5
         *                                                       (senf::ListBParser< IPv6Packet, num_of_fbips>) ); //6
         * This can't work for two reasons: 
         *      -SENF_PARSER_PRIVATE_VARIANT only accepts 6 templates in types but you have to start from 0.
         *      -you NEVER can use templated Parsers in these macros since the macro-preprocessor won't recognize the <> brackets and will
         *      interpret the ","
         * 
         * The first problem is solved by using key()
         * The second problem is solved by introducing Helper-Parser which cover both the list and the number field. By that no 
         *      templates have to be used. 
         */

        SENF_PARSER_VARIANT( fbiplist, ip_version,
       	                         ( ids(getIpv4AddressList, na, setIpVersion4, 
				       key(4, senf::DTCPIPv4AddressListParser)) )    //IPv4 
  			         ( ids(getIpv6AddressList, na, setIpVersion6,
				       key(6, senf::DTCPIPv6AddressListParser)) ) ); //IPv6
                                                                 
        SENF_PARSER_FINALIZE(DTCPPacketParser);
    };
    
    /** \brief DTCP packet
        
        \par Packet type (typedef):
            \ref DTCPPacket

        \par Fields:
            \ref DTCPPacketParser

        \ingroup protocolbundle_mpegdvb
     */
    struct DTCPPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<DTCPPacketType>
    {
        typedef PacketTypeMixin<DTCPPacketType> mixin;
        typedef ConcretePacket<DTCPPacketType> packet;
        typedef DTCPPacketParser parser;
    
        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
        
        static void dump(packet p, std::ostream & os);
    };
    
    /** \brief DTCP packet typedef */
    typedef DTCPPacketType::packet DTCPPacket;
}

#endif /*DTCPPACKET_HH_*/
