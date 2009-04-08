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
    struct IPv6PacketParserExtension_Fragment : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD            ( nextHeader     , UInt8Parser  );
        SENF_PARSER_PRIVATE_FIELD    ( reserved1      , UInt8Parser  );

        SENF_PARSER_BITFIELD         ( fragmentOffset , 13, unsigned );
        SENF_PARSER_PRIVATE_BITFIELD ( reserved2      ,  2, unsigned );
        SENF_PARSER_BITFIELD         ( moreFragments  ,  1, bool     );

        SENF_PARSER_FIELD            ( id             , UInt32Parser );

        SENF_PARSER_FINALIZE(IPv6PacketParserExtension_Fragment);
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
    struct IPv6ExtensionType_Fragment
        : public PacketTypeBase,
          public PacketTypeMixin<IPv6ExtensionType_Fragment, IpTypes>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<IPv6ExtensionType_Fragment, IpTypes> mixin;
#endif
        /** \brief IPv6 fragment extension packet typedef */
        typedef ConcretePacket<IPv6ExtensionType_Fragment> packet; 
        /** \brief typedef to the parser of IPv6 fragment extension packet */
        typedef IPv6PacketParserExtension_Fragment parser;

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
    typedef ConcretePacket<IPv6ExtensionType_Fragment> IPv6Extension_Fragment;

// =====================================================================================================

    /** \brief Parse in IPv6 routing extension header

        Parser implementing the IPv6 routing Header extension. The fields implemented are:
    \image html IPv6Extensions_Routing.png

    \see IPv6ExtensionType_Routing \n
    <a href="http://tools.ietf.org/html/rfc2460">RFC 2460</a>
     */

//Routing Header Extension (type 0 only)
    struct IPv6PacketParserExtension_Routing : public PacketParserBase
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
        
        SENF_PARSER_FIELD ( nextHeader, UInt8Parser	);
        SENF_PARSER_FIELD ( headerLength, UInt8Parser	);
        SENF_PARSER_FIELD ( routingType, UInt8Parser	); //set to Zero for minimal implementation
        SENF_PARSER_FIELD_RO ( segmentsLeft, UInt8Parser   );
        SENF_PARSER_FIELD ( reserved, UInt32Parser      ); //set to zero by RFC
        SENF_PARSER_VECTOR ( hopAddresses, segmentsLeft, INet6AddressParser );
        
        SENF_PARSER_FINALIZE ( IPv6PacketParserExtension_Routing );
        
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
    
    struct IPv6ExtensionType_Routing
    :   public PacketTypeBase,
        public PacketTypeMixin<IPv6ExtensionType_Routing, IpTypes>

    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<IPv6ExtensionType_Routing, IpTypes> mixin;
#endif
        /** \brief IPv6 routing extension packet typedef */
        typedef ConcretePacket<IPv6ExtensionType_Routing> packet;
        /** \brief typedef to the parser of IPv6 routing extension packet */
        typedef IPv6PacketParserExtension_Routing parser;
        
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
    typedef ConcretePacket<IPv6ExtensionType_Routing> IPv6Extension_Routing;

    
// =====================================================================================================
    
    /** \brief Parse in IPv6 Hop-By-Hop extension header

        Parser implementing the IPv6 routing Header extension. The fields implemented are:
    \image html IPv6Extensions_HopByHop.png

    \see IPv6ExtensionType_HopByHop \n
    <a href="http://tools.ietf.org/html/rfc2460">RFC 2460</a>
    */

// Hop-By-Hop skeleton without Options 

    struct IPv6PacketParserExtension_HopByHop : public PacketParserBase {
#       include SENF_PARSER()
        SENF_PARSER_FIELD ( nextHeader, UInt8Parser	);
        SENF_PARSER_FIELD ( headerLength, UInt8Parser	);
        
        SENF_PARSER_FINALIZE ( IPv6PacketParserExtension_HopByHop );
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
    
    struct IPv6ExtensionType_HopByHop
    :   public PacketTypeBase,
        public PacketTypeMixin<IPv6ExtensionType_HopByHop, IpTypes>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<IPv6ExtensionType_HopByHop, IpTypes> mixin;
#endif
        /** \brief IPv6 Hop-By-Hop extension packet typedef */
        typedef ConcretePacket<IPv6ExtensionType_HopByHop> packet;
        /** \brief typedef to the parser of IPv6 Hop-By-Hop extension packet */
        typedef IPv6PacketParserExtension_HopByHop parser;
        
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
    typedef ConcretePacket<IPv6ExtensionType_Routing> IPv6Extension_HopByHop;
    
} //namespace senf
///////////////////////////////hh.e////////////////////////////////////////
#endif
#ifndef SENF_PACKETS_DECL_ONLY
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
