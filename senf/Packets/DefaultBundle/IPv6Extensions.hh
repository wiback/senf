// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Stefan Bund <g0dil@berlios.de>
//   Philipp Batroff <pug@berlios.de>
/** \file
    \brief IPv6Extensions public header */

#ifndef HH_SENF_Packets_DefaultBundle_IPv6Extensions_
#define HH_SENF_Packets_DefaultBundle_IPv6Extensions_ 1

// Custom includes
#include "IPv6ExtOptions.hh"
#include "ListOptionTypeParser.hh"
#include "IPv6Packet.hh"

//#include "IPv6Extensions.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Parse in IPv6 fragment extension header

        Parser implementing the IPv6 fragment extension. The fields implemented are:
        \image html IPv6FragmentPacket.png

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
            \ref IPv6FragmentPacket

        \par Fields:
            \ref IPv6FragmentPacketParser

        \par Associated registries:
            \ref IpTypes

        \par Finalize action:
            Set \a nextHeader from type of next packet if found in \ref IpTypes

        \ingroup protocolbundle_default
     */
    struct IPv6FragmentPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<IPv6FragmentPacketType, IpTypes>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<IPv6FragmentPacketType, IpTypes> mixin;
#endif
        /** \brief IPv6 fragment extension packet typedef */
        typedef ConcretePacket<IPv6FragmentPacketType> packet;
        /** \brief typedef to the parser of IPv6 fragment extension packet */
        typedef IPv6FragmentPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::initSize;
        using mixin::init;

        static key_t nextPacketKey(packet p)
            { return p->nextHeader(); }

        /** \brief Dump given IPv6FragmentPacket in readable form to given output stream */
        static void dump(packet p, std::ostream & os);

        static void finalize(packet p);
    };

    /** \brief IPv6 fragment extension packet typedef
        \ingroup protocolbundle_default
     */
    typedef ConcretePacket<IPv6FragmentPacketType> IPv6FragmentPacket;

// =====================================================================================================

    /** \brief Parse in IPv6 routing extension header

        Parser implementing the IPv6 routing Header extension (type 0 only).
        The fields implemented are:
        \image html IPv6RoutingPacket.png

        \see IPv6ExtensionType_Routing \n
            <a href="http://tools.ietf.org/html/rfc2460">RFC 2460</a>
     */
    struct IPv6RoutingPacketParser : public PacketParserBase
    {
        /*
        The Type 0 Routing header has the following format: (RFC 2460)

        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        |  Next Header  |  Hdr Ext Len  | Routing Type=0| Segments Left |
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        |                            Reserved                           |
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        |                                                               |
        +
        |                                                               |
        +                           Address[1]
        |                                                               |
        +
        |                                                               |
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        .                               .                               .
        .                               .                               .
        .                               .                               .
        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        |                                                               |
        +
        |                                                               |
        +                           Address[n]
        |                                                               |
        +
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

        SENF_PARSER_FINALIZE ( IPv6RoutingPacketParser );

        //provisionary, since only type 0 is implemented
        SENF_PARSER_INIT() {
            routingType() = 0u;
            reserved() = 0u;
        }
    };

    /** \brief IPv6 routing extension

        \par Packet type (typedef):
            \ref IPv6RoutingPacket

        \par Fields:
            \ref IPv6RoutingPacketParser

        \par Associated registries:
            \ref IpTypes

        \par Finalize action:
            Set \a nextHeader from type of next packet if found in \ref IpTypes

        \ingroup protocolbundle_default
     */
    struct IPv6RoutingPacketType
    :   public PacketTypeBase,
        public PacketTypeMixin<IPv6RoutingPacketType, IpTypes>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<IPv6RoutingPacketType, IpTypes> mixin;
#endif
        /** \brief IPv6 routing extension packet typedef */
        typedef ConcretePacket<IPv6RoutingPacketType> packet;
        /** \brief typedef to the parser of IPv6 routing extension packet */
        typedef IPv6RoutingPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::init;
        using mixin::initSize;

        static key_t nextPacketKey(packet p)
            { return p->nextHeader(); }

        /** \brief Dump given IPv6RoutingPacket in readable form to given output stream */
        static void dump(packet p, std::ostream & os);

        static void finalize(packet p);
    };

    /** \brief IPv6 routing extension packet typedef
        \ingroup protocolbundle_default
     */
    typedef ConcretePacket<IPv6RoutingPacketType> IPv6RoutingPacket;


// =====================================================================================================

    /** \brief Parse in IPv6 Hop-By-Hop extension header

        Parser implementing the IPv6 routing Header extension. The fields implemented are:
        \image html IPv6HopByHopOptionsPacket.png

        \see IPv6ExtensionType_HopByHop \n
            <a href="http://tools.ietf.org/html/rfc2460">RFC 2460</a>
    */
    struct IPv6HopByHopOptionsPacketParser : public PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_FIELD    ( nextHeader, UInt8Parser   );
        SENF_PARSER_FIELD_RO ( headerLength, UInt8Parser );

        typedef detail::FixedAuxParserPolicy<UInt8Parser, 1u> ListOptionTypeAuxPolicy;
        typedef detail::ListOptionTypeParser_Policy<
            IPv6GenericOptionParser, ListOptionTypeAuxPolicy> ListOptionTypePolicy;
        typedef ListParser<ListOptionTypePolicy> ListOptionTypeParser;

        SENF_PARSER_FIELD  ( options, ListOptionTypeParser);

        SENF_PARSER_FINALIZE ( IPv6HopByHopOptionsPacketParser );
    };

    /** \brief IPv6 Hop-By-Hop extension

        \par Packet type (typedef):
            \ref IPv6HopByHopOptionsPacket

        \par Fields:
            \ref IPv6HopByHopOptionsPacketParser

        \par Associated registries:
            \ref IpTypes

        \par Finalize action:
            Set \a nextHeader from type of next packet if found in \ref IpTypes

        \ingroup protocolbundle_default
    */
    struct IPv6HopByHopOptionsPacketType
    :   public PacketTypeBase,
        public PacketTypeMixin<IPv6HopByHopOptionsPacketType, IpTypes>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<IPv6HopByHopOptionsPacketType, IpTypes> mixin;
#endif
        /** \brief IPv6 Hop-By-Hop extension packet typedef */
        typedef ConcretePacket<IPv6HopByHopOptionsPacketType> packet;
        /** \brief typedef to the parser of IPv6 Hop-By-Hop extension packet */
        typedef IPv6HopByHopOptionsPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::init;
        using mixin::initSize;

        static key_t nextPacketKey(packet p) {
            return p->nextHeader(); }

        /** \brief Dump given IPv6HopByHopOptionsPacket in readable form to given output stream */
        static void dump(packet p, std::ostream & os);

        static void finalize(packet p);
    };

    /** \brief IPv6 routing Hop-By-Hop packet typedef
        \ingroup protocolbundle_default
     */
    typedef ConcretePacket<IPv6HopByHopOptionsPacketType> IPv6HopByHopOptionsPacket;

// =====================================================================================================

    /** \brief Parse in IPv6 Destination Options extension header

        Parser implementing the IPv6 Destination Options Header extension.
        The fields implemented are:
        \image html IPv6DestinationOptionsPacket.png

        \see IPv6ExtensionType_Destination \n
            <a href="http://tools.ietf.org/html/rfc2460">RFC 2460</a>
     */
    struct IPv6DestinationOptionsPacketParser : public PacketParserBase
    {
#       include SENF_PARSER()
        SENF_PARSER_FIELD ( nextHeader, UInt8Parser   );
        SENF_PARSER_FIELD_RO ( headerLength, UInt8Parser );
        typedef detail::FixedAuxParserPolicy<UInt8Parser, 1u> ListOptionTypeAuxPolicy;
        typedef detail::ListOptionTypeParser_Policy<
            IPv6GenericOptionParser, ListOptionTypeAuxPolicy> ListOptionTypePolicy;
        typedef ListParser<ListOptionTypePolicy> ListOptionTypeParser;

        SENF_PARSER_FIELD  ( options, ListOptionTypeParser);

        SENF_PARSER_FINALIZE ( IPv6DestinationOptionsPacketParser );
    };

    /** \brief IPv6 Destination Options extension

        \par Packet type (typedef):
            \ref IPv6DestinationOptionsPacket

        \par Fields:
            \ref IPv6DestinationOptionsPacketParser

        \par Associated registries:
            \ref IpTypes

        \par Finalize action:
            Set \a nextHeader from type of next packet if found in \ref IpTypes

        \ingroup protocolbundle_default
     */
    struct IPv6DestinationOptionsPacketType
    :   public PacketTypeBase,
        public PacketTypeMixin<IPv6DestinationOptionsPacketType, IpTypes>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<IPv6DestinationOptionsPacketType, IpTypes> mixin;
#endif
        /** \brief IPv6 Destination Options extension packet typedef */
        typedef ConcretePacket<IPv6DestinationOptionsPacketType> packet;
        /** \brief typedef to the parser of IPv6 Destination Options extension packet */
        typedef IPv6DestinationOptionsPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::init;
        using mixin::initSize;

        static key_t nextPacketKey(packet p)
        { return p->nextHeader(); }
        /** \brief Dump given IPv6DestinationOptionsPacket in readable form to given output stream */
        static void dump(packet p, std::ostream & os);

        static void finalize(packet p);
    };

    /** \brief IPv6 routing Destination Options packet typedef
        \ingroup protocolbundle_default
     */
    typedef ConcretePacket<IPv6DestinationOptionsPacketType> IPv6DestinationOptionsPacket;

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
