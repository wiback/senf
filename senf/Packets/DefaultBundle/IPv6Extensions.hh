// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
//     Philipp Batroff <philipp.batroff@fokus.fraunhofer.de>
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
    \brief IPv6Extensions public header */

#ifndef HH_SENF_Packets_DefaultBundle_IPv6Extensions_
#define HH_SENF_Packets_DefaultBundle_IPv6Extensions_ 1

// Custom includes
#include <senf/Packets/Packets.hh>
#include <senf/Packets/AuxParser.hh>
#include <senf/Packets/DefaultBundle/IPv6ExtOptionType.hh>
#include "ListOptionTypeParser.hh"
#include "IPv6Packet.hh"

//#include "IPv6Extensions.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Parse in IPv6 fragment extension header
        
        Parser implementing the IPv6 fragment extension. The fields implemented are:
        \image html IPv6Extensions_Fragment.png

        \see IPv6ExtensionType_Fragment \n
            <a href="http://tools.ietf.org/html/rfc2460">RFC 2460</a>
     */
    struct IPv6FragmentPacketParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD            ( nextHeader     , UInt8Parser  );
        SENF_PARSER_PRIVATE_FIELD    ( reserved1      , UInt8Parser  );
        SENF_PARSER_BITFIELD         ( fragmentOffset , 13, unsigned );
        SENF_PARSER_PRIVATE_BITFIELD ( reserved2      ,  2, unsigned );
        SENF_PARSER_BITFIELD         ( moreFragments  ,  1, bool     );
        SENF_PARSER_FIELD            ( id             , UInt32Parser );

        SENF_PARSER_FINALIZE(IPv6FragmentPacketParser);
    };

    /** \brief IPv6 fragment extension

        \par Packet type (typedef):
            \ref IPv6Extension_Fragment

        \par Fields:
            \ref IPv6PacketParserExtension_Fragment
        
        \par Associated registries:
            \ref IpTypes
        
        \par Finalize action:
            Set \a nextHeader from type of next packet if found in \ref IpTypes

        \ingroup protocolbundle_default
     */
    struct IPv6FragmentType
        : public PacketTypeBase,
          public PacketTypeMixin<IPv6FragmentType, IpTypes>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<IPv6FragmentType, IpTypes> mixin;
#endif
        /** \brief IPv6 fragment extension packet typedef */
        typedef ConcretePacket<IPv6FragmentType> packet;
        /** \brief typedef to the parser of IPv6 fragment extension packet */
        typedef IPv6FragmentPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::initSize;
        using mixin::init;

        static key_t nextPacketKey(packet p) 
            { return p->nextHeader(); }
        
        /** \brief Dump given IPv6Extension_Fragment in readable form to given output stream */
        static void dump(packet p, std::ostream & os); 

        static void finalize(packet p) { 
            p->nextHeader() << key(p.next(nothrow)); }
    };

    /** \brief IPv6 fragment extension packet typedef */
    typedef ConcretePacket<IPv6FragmentType> IPv6Fragment;

// =====================================================================================================

    /** \brief Parse in IPv6 routing extension header

        Parser implementing the IPv6 routing Header extension. The fields implemented are:
        \image html IPv6Extensions_Routing.png

        \see IPv6ExtensionType_Routing \n
        <a href="http://tools.ietf.org/html/rfc2460">RFC 2460</a>
     */

//Routing Header Extension (type 0 only)
    struct IPv6PRoutingPacketParser : public PacketParserBase
    {
/*
The Type 0 Routing header has the following format: (RFC 2460)

    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |  Next Header  |  Hdr Ext Len  | Routing Type=0| Segments Left |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                            Reserved                           |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                                                               |
    +                                                               +
    |                                                               |
    +                           Address[1]                          +
    |                                                               |
    +                                                               +
    |                                                               |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    .                               .                               .
    .                               .                               .
    .                               .                               .
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                                                               |
    +                                                               +
    |                                                               |
    +                           Address[n]                          +
    |                                                               |
    +                                                               +
    |                                                               |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

*/
#       include SENF_PARSER()
        
        SENF_PARSER_FIELD ( nextHeader, UInt8Parser      );
        SENF_PARSER_FIELD ( headerLength, UInt8Parser    );
        SENF_PARSER_FIELD ( routingType, UInt8Parser     ); //set to Zero for minimal implementation
        SENF_PARSER_FIELD_RO ( segmentsLeft, UInt8Parser );
        SENF_PARSER_FIELD ( reserved, UInt32Parser       ); //set to zero by RFC
        SENF_PARSER_VECTOR ( hopAddresses, segmentsLeft, INet6AddressParser );
        
        SENF_PARSER_FINALIZE ( IPv6PRoutingPacketParser );
        
        //provisionary, since only type 0 is implemented
        SENF_PARSER_INIT() { 
            routingType() = 0u;
            reserved() = 0u; 
        }
    };
    
    /** \brief IPv6 routing extension

        \par Packet type (typedef):
    \ref IPv6Extension_Routing

        \par Fields:
    \ref IPv6PacketParserExtension_Routing
        
        \par Associated registries:
    \ref IpTypes
        
        \par Finalize action:
    Set \a nextHeader from type of next packet if found in \ref IpTypes

    \ingroup protocolbundle_default
         */
    
    struct IPv6RoutingType
    :   public PacketTypeBase,
        public PacketTypeMixin<IPv6RoutingType, IpTypes>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<IPv6RoutingType, IpTypes> mixin;
#endif
        /** \brief IPv6 routing extension packet typedef */
        typedef ConcretePacket<IPv6RoutingType> packet;
        /** \brief typedef to the parser of IPv6 routing extension packet */
        typedef IPv6PRoutingPacketParser parser;
        
        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::init;
        using mixin::initSize;
        
        static key_t nextPacketKey(packet p) 
            { return p->nextHeader(); }
        /** \brief Dump given IPv6Extension_Routing in readable form to given output stream */
        static void dump(packet p, std::ostream & os); 
        
        static void finalize(packet p) { 
            p->nextHeader() << key(p.next(nothrow)); }
    };
    
    /** \brief IPv6 routing extension packet typedef */
    typedef ConcretePacket<IPv6RoutingType> IPv6Routing;

    
// =====================================================================================================
    
    /** \brief Parse in IPv6 Hop-By-Hop extension header

        Parser implementing the IPv6 routing Header extension. The fields implemented are:
    \image html IPv6Extensions_HopByHop.png

    \see IPv6ExtensionType_HopByHop \n
    <a href="http://tools.ietf.org/html/rfc2460">RFC 2460</a>
    */

// Hop-By-Hop Extension Header

    struct IPv6HopByHopPacketParser : public PacketParserBase {
#       include SENF_PARSER()
        SENF_PARSER_FIELD ( nextHeader, UInt8Parser   );
        SENF_PARSER_FIELD ( headerLength, UInt8Parser );

        typedef detail::FixedAuxParserPolicy<UInt8Parser, 1u> ListOptionTypeAuxPolicy;
        typedef detail::ListOptionTypeParser_Policy<GenericOptTypeTLVParser, ListOptionTypeAuxPolicy> ListOptionTypePolicy;
        typedef ListParser<ListOptionTypePolicy> ListOptionTypeParser;

        SENF_PARSER_FIELD  ( options, ListOptionTypeParser);

        SENF_PARSER_FINALIZE ( IPv6HopByHopPacketParser );
    };
    
    /** \brief IPv6 Hop-By-Hop extension

        \par Packet type (typedef):
    \ref IPv6Extension_HopByHop

        \par Fields:
    \ref IPv6PacketParserExtension_HopByHop
        
        \par Associated registries:
    \ref IpTypes
        
        \par Finalize action:
    Set \a nextHeader from type of next packet if found in \ref IpTypes

    \ingroup protocolbundle_default
    */
    
    struct IPv6HopByHopType
    :   public PacketTypeBase,
        public PacketTypeMixin<IPv6HopByHopType, IpTypes>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<IPv6HopByHopType, IpTypes> mixin;
#endif
        /** \brief IPv6 Hop-By-Hop extension packet typedef */
        typedef ConcretePacket<IPv6HopByHopType> packet;
        /** \brief typedef to the parser of IPv6 Hop-By-Hop extension packet */
        typedef IPv6HopByHopPacketParser parser;
        
        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::init;
        using mixin::initSize;
        
        static key_t nextPacketKey(packet p) 
        { return p->nextHeader(); }
        /** \brief Dump given IPv6Extension_HopByHop in readable form to given output stream */
        static void dump(packet p, std::ostream & os); 
        
        static void finalize(packet p) { 
            p->nextHeader() << key(p.next(nothrow)); }
    };
    
    /** \brief IPv6 routing Hop-By-Hop packet typedef */
    typedef ConcretePacket<IPv6HopByHopType> IPv6HopByHop;
    
// =====================================================================================================
    
    /** \brief Parse in IPv6 Destination Options extension header

        Parser implementing the IPv6 Destination Options Header extension. The fields implemented are:
    \image html IPv6Extensions_Destination.png

    \see IPv6ExtensionType_Destination \n
    <a href="http://tools.ietf.org/html/rfc2460">RFC 2460</a>
     */
    
    // Destination Options skeleton without TLV-Options

    struct IPv6DestinationPacketParser : public PacketParserBase {

#       include SENF_PARSER()
        SENF_PARSER_FIELD ( nextHeader, UInt8Parser   );
        SENF_PARSER_FIELD ( headerLength, UInt8Parser );
        
        SENF_PARSER_FINALIZE ( IPv6DestinationPacketParser );
    };
    
    /** \brief IPv6 Destination Options extension

        \par Packet type (typedef):
    \ref IPv6Extension_Destination

        \par Fields:
    \ref IPv6PacketParserExtension_Destination
        
        \par Associated registries:
    \ref IpTypes
        
        \par Finalize action:
    Set \a nextHeader from type of next packet if found in \ref IpTypes

    \ingroup protocolbundle_default
         */
    
    struct IPv6DestinationType
    :   public PacketTypeBase,
        public PacketTypeMixin<IPv6DestinationType, IpTypes>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<IPv6DestinationType, IpTypes> mixin;
#endif
        /** \brief IPv6 Destination Options extension packet typedef */
        typedef ConcretePacket<IPv6DestinationType> packet;
        /** \brief typedef to the parser of IPv6 Destination Options extension packet */
        typedef IPv6DestinationPacketParser parser;
        
        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::init;
        using mixin::initSize;
        
        static key_t nextPacketKey(packet p) 
        { return p->nextHeader(); }
        /** \brief Dump given IPv6Extension_Destination in readable form to given output stream */
        static void dump(packet p, std::ostream & os); 
        
        static void finalize(packet p) { 
            p->nextHeader() << key(p.next(nothrow)); }
    };
    
    /** \brief IPv6 routing Destination Options packet typedef */
    typedef ConcretePacket<IPv6DestinationType> IPv6Destination;

    
} //namespace senf
///////////////////////////////hh.e////////////////////////////////////////
//#include "IPv6Extensions.cci"
//#include "IPv6Extensions.ct"
//#include "IPv6Extensions.cti"
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
