// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum NETwork research (NET)
//     David Wagner <david.wagner@fokus.fraunhofer.de>
//
#ifndef DTCPPACKET_HH_
#define DTCPPACKET_HH_

#include "../../Packets/Packets.hh"
#include "../../Packets/DefaultBundle/IPv4Packet.hh"
#include "../../Packets/DefaultBundle/IPv6Packet.hh"

namespace senf {
    
    //first we have to define some helpers
    struct DTCPIPv4AddressListParser : public PacketParserBase {
#       include SENF_PARSER()        
        SENF_PARSER_PRIVATE_FIELD ( num_of_fbips, Parse_UInt8 );
        SENF_PARSER_PRIVATE_FIELD ( reserved ,    Parse_UInt8 );   //must be zero 
        SENF_PARSER_VEC_N         ( fbiplist,     num_of_fbips, Parse_INet4Address );

	SENF_PARSER_FINALIZE(DTCPIPv4AddressListParser);
    };
        
    struct DTCPIPv6AddressListParser : public PacketParserBase {
#       include SENF_PARSER()        
        SENF_PARSER_PRIVATE_FIELD ( num_of_fbips, Parse_UInt8 );
        SENF_PARSER_PRIVATE_FIELD ( reserved,     Parse_UInt8 );   //must be zero 
        SENF_PARSER_VEC_N         ( fbiplist,     num_of_fbips, Parse_INet6Address );

	SENF_PARSER_FINALIZE(DTCPIPv6AddressListParser);
    };

    /** \brief Parse a DTCP packet

        Parser implementing the DTCP packet according to RFC 3077
        
        \see DTCPPacketType
     */
    struct Parse_DTCPPacket : public PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_BITFIELD         ( version_number,       4, unsigned );  // =1 according to rfc3077
        SENF_PARSER_BITFIELD         ( command,              4, unsigned );  // 1=JOIN 2=LEAVE
        SENF_PARSER_FIELD            ( interval,             Parse_UInt8 );  // 5 according to rfc3077
        SENF_PARSER_FIELD            ( sequence_number,      Parse_UInt16 );
        SENF_PARSER_PRIVATE_BITFIELD ( reserved,             3, unsigned );
        SENF_PARSER_BITFIELD         ( receive_capable_feed, 1, bool );      // 0=send only, 1=receive_capable_feed
        SENF_PARSER_BITFIELD         ( ip_version,           4, unsigned );  // 4=IPv4, 6=IPv6
        SENF_PARSER_FIELD            ( tunnel_protocol,      Parse_UInt8 ); 
		/* Please consider the following comments on the implementation given in this class: 
		 * 1. you could think of simply using SENF_PARSER_PRIVATE_VARIANT and List / Vectorparser like this:
		 * SENF_PARSER_PRIVATE_VARIANT  ( fbiplist,             ip_version,
         *                                                       (senf::VoidPacketParser) //ip_version=0
         *                                                       (senf::VoidPacketParser) //1
         *                                                       (senf::VoidPacketParser) //2
         *                                                       (senf::VoidPacketParser) //3
         *                                                       (senf::Parse_ListB< IPv4Packet, num_of_fbips>) //4 
         *                                                       (senf::VoidPacketParser) //5
         *                                                       (senf::Parse_ListB< IPv6Packet, num_of_fbips>) ); //6
         * This can't work for two reasons: 
         * 		-SENF_PARSER_PRIVATE_VARIANT only accepts 6 templates in types but you have to start from 0.
         * 		-you NEVER can use templated Parsers in these macros since the macro-preprocessor won't recognize the <> brackets and will
         * 			interpret the ","
         * 
         * The first problem is solved by using (actually inventing)  SENF_PARSER_VARIANT_TRANS which has the same limitations 
         * 		concerning the number of types but isn't limited to the values used. This is achieved by a translating function 
         * 		as you can see. 
         * The second problem is solved by introducing Helper-Parser which cover both the list and the number field. By that no 
         * 		templates have to be used. 
		*/

		struct ip_version_translator {
		    static unsigned fromChooser(ip_version_t::value_type in) {
			switch (in) { 
			case 4: return 0;
			case 6: return 1;
			}
		    }
		    static ip_version_t::value_type toChooser(unsigned in) {
			switch (in) {
			    case 0: return 4;
			    case 1: return 6; 
			}
		    }
		};
    
        SENF_PARSER_VARIANT_TRANS    ( fbiplist,             ip_version, ip_version_translator,
                                                                 (senf::DTCPIPv4AddressListParser)        //IPv4 
                                                                 (senf::DTCPIPv6AddressListParser) );     //IPv6

	SENF_PARSER_FINALIZE(Parse_DTCPPacket);
    };
    
    /** \brief DTCP packet
        
        \par Packet type (typedef):
            \ref DTCPPacket

        \par Fields:
            \ref Parse_DTCPPacket

        \ingroup protocolbundle_mpegdvb
     */
    struct DTCPPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<DTCPPacketType>
    {
        typedef PacketTypeMixin<DTCPPacketType> mixin;
        typedef ConcretePacket<DTCPPacketType> packet;
        typedef Parse_DTCPPacket parser;
    
        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
        
        static void dump(packet p, std::ostream & os);
    };
    
    /** \brief DTCP packet typedef */
    typedef DTCPPacketType::packet DTCPPacket;
}

#endif /*DTCPPACKET_HH_*/
