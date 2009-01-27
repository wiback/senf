// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
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

        \see IPv6ExtensionType_Fragment \n
            <a href="http://tools.ietf.org/html/rfc2460">RFC 2460</a>
     */
    struct IPv6PacketParserExtension_Fragment : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD( nextHeader     , UInt8Parser );
        SENF_PARSER_FIELD( reserved1      , UInt8Parser );

        SENF_PARSER_BITFIELD( fragmentOffset, 13, unsigned );
        SENF_PARSER_BITFIELD( reserved2,       2, unsigned );
        SENF_PARSER_BITFIELD( moreFragments,   1, bool     );

        SENF_PARSER_FIELD( id             , UInt32Parser   );

        SENF_PARSER_FINALIZE(IPv6PacketParserExtension_Fragment);
    };

    /** \brief IPv6 fragment extension

        \par Packet type (typedef):
            \ref IPv6Extension_Fragment

        \par Fields:
            \ref IPv6PacketParserExtension_Fragment
        
        \par Associated registries:
            \par IpTypes
        
        \par Finalize action:
            Set \a nextHeader from type of next packet if found in \ref IpTypes

        \image html IPv6Extensions_Fragment.png

        \ingroup protocolbundle_default
     */
    struct IPv6ExtensionType_Fragment
        : public PacketTypeBase,
          public PacketTypeMixin<IPv6ExtensionType_Fragment, IpTypes>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<IPv6ExtensionType_Fragment, IpTypes> mixin;
        typedef ConcretePacket<IPv6ExtensionType_Fragment> packet;
        typedef IPv6PacketParserExtension_Fragment parser;
#endif
        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::initSize;
        using mixin::init;

        static key_t nextPacketKey(packet p) 
            { return p->nextHeader(); }
        
        static void dump(packet p, std::ostream & os);

        static void finalize(packet p)
            { p->nextHeader() << key(p.next(nothrow)); }
    };

    /** \brief IPv6 fragment extension packet typedef */
    typedef ConcretePacket<IPv6ExtensionType_Fragment> IPv6Extension_Fragment;
}

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
