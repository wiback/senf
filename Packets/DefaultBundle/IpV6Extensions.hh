// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief IpV6Extensions public header */

#ifndef HH_IpV6Extensions_
#define HH_IpV6Extensions_ 1

// Custom includes
#include "IpV6Packet.hh"

//#include "IpV6Extensions.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Parse in IpV6 fragment extension header
        
        Parser implementing the IpV6 fragment extension. The fields implemented are:

        \see IpV6ExtensionType_Fragment \n
            <a href="http://tools.ietf.org/html/rfc2460">RFC 2460</a>
     */
    struct Parse_IpV6Extension_Fragment : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD( nextHeader     , Parse_UInt8 );
        SENF_PARSER_FIELD( reserved1      , Parse_UInt8 );

        SENF_PARSER_BITFIELD( fragmentOffset, 13, unsigned );
        SENF_PARSER_BITFIELD( reserved2,       2, unsigned );
        SENF_PARSER_BITFIELD( moreFragments,   1, bool     );

        SENF_PARSER_FIELD( id             , Parse_UInt32   );

        SENF_PARSER_FINALIZE(Parse_IpV6Extension_Fragment);
    };

    /** \brief IpV6 fragment extension

        \par Packet type (typedef):
            \ref IpV6Extension_Fragment

        \par Fields:
            \ref Parse_IpV6Extension_Fragment
        
        \par Associated registries:
            \par IpTypes
        
        \par Finalize action:
            Set \a nextHeader from type of next packet if found in \ref IpTypes

        \ingroup protocolbundle_default
     */
    struct IpV6ExtensionType_Fragment
        : public PacketTypeBase,
          public PacketTypeMixin<IpV6ExtensionType_Fragment, IpTypes>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<IpV6ExtensionType_Fragment, IpTypes> mixin;
        typedef ConcretePacket<IpV6ExtensionType_Fragment> packet;
        typedef Parse_IpV6Extension_Fragment parser;
#endif
        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::initSize;
        using mixin::init;

        static registry_key_t nextPacketKey(packet p) 
            { return p->nextHeader(); }
        
        static void dump(packet p, std::ostream & os);

        static void finalize(packet p)
            { p->nextHeader() << key(p.next()); }
    };

    /** \brief IpV6 fragment extension packet typedef */
    typedef ConcretePacket<IpV6ExtensionType_Fragment> IpV6Extension_Fragment;
}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#ifndef SENF_PACKETS_DECL_ONLY
//#include "IpV6Extensions.cci"
//#include "IpV6Extensions.ct"
//#include "IpV6Extensions.cti"
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
