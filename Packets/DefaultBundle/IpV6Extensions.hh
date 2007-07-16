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

    // See RFC2460
    struct Parse_IpV6Extension_Fragment : public PacketParserBase
    {
        SENF_PACKET_PARSER_INIT(Parse_IpV6Extension_Fragment);

        ///////////////////////////////////////////////////////////////////////////

        typedef Parse_UInt8                Parse_8bit;
        typedef Parse_UIntField <  0, 13 > Parse_Offset;
        typedef Parse_UIntField < 13, 15 > Parse_Reserved;
        typedef Parse_Flag      < 15     > Parse_More;
        typedef Parse_UInt32               Parse_32bit;

        SENF_PACKET_PARSER_DEFINE_FIXED_FIELDS(
            ((Field       )( nextHeader     , Parse_8bit     ))
            ((Field       )( reserved1      , Parse_8bit     ))
            ((OverlayField)( fragmentOffset , Parse_Offset   ))
            ((OverlayField)( reserved2      , Parse_Reserved ))
            ((Field       )( moreFragments  , Parse_More     ))
            ((Field       )( id             , Parse_32bit    )) );
    };

    struct IpV6ExtensionType_Fragment
        : public PacketTypeBase,
          public PacketTypeMixin<IpV6ExtensionType_Fragment, IpTypes>
    {
        typedef PacketTypeMixin<IpV6ExtensionType_Fragment, IpTypes> mixin;
        typedef ConcretePacket<IpV6ExtensionType_Fragment> packet;
        typedef Parse_IpV6Extension_Fragment parser;

        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::initSize;
        using mixin::init;

        static registry_key_t nextPacketKey(packet p) 
            { return p->nextHeader(); }
        
        static void dump(packet p, std::ostream & os);
    };

    typedef IpV6ExtensionType_Fragment::packet IpV6Extension_Fragment;
}

///////////////////////////////hh.e////////////////////////////////////////
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
