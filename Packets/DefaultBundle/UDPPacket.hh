// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

#ifndef HH_UDPPacket_
#define HH_UDPPacket_ 1

// Custom includes
#include "Packets/Packets.hh"

//#include "UDPPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    ///\addtogroup protocolbundle_default
    ///@{

    // See RFC768
    struct Parse_UDP : public PacketParserBase
    {
        SENF_PACKET_PARSER_INIT(Parse_UDP);

        ///////////////////////////////////////////////////////////////////////////

        typedef Parse_UInt16 Parse_16bit;

        SENF_PACKET_PARSER_DEFINE_FIXED_FIELDS(
            ((Field)( source,      Parse_16bit ))
            ((Field)( destination, Parse_16bit ))
            ((Field)( length,      Parse_16bit ))
            ((Field)( crc,         Parse_16bit )) );
    };

    struct UDPPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<UDPPacketType>
    {
        typedef PacketTypeMixin<UDPPacketType> mixin;
        typedef ConcretePacket<UDPPacketType> packet;
        typedef Parse_UDP parser;

        using mixin::nextPacketRange;
        using mixin::initSize;
        using mixin::init;

        static void dump(packet p, std::ostream & os);
    };

    typedef UDPPacketType::packet UDPPacket;

    ///@}
}


///////////////////////////////hh.e////////////////////////////////////////
#endif
#ifndef SENF_PACKETS_DECL_ONLY
//#include UDPPacket.cci"
//#include "UDPPacket.ct"
//#include "UDPPacket.cti"
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
