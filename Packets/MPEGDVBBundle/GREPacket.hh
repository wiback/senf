// $Id:GREPacket.hh 327 2007-07-20 10:03:44Z tho $
//
// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Joachim Kaeber <joachim.kaeber@fokus.fraunhofer.de>
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
    \brief GREPacket public header */

#ifndef HH_GREPacket
#define HH_GREPacket 1

// Custom includes
#include <algorithm>
#include "../../Packets/Packets.hh"

//#include "GREPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Parse a GRE packet

        Parser implementing the header of a General Routing Encapsulation (GRE, RFC 2784) Packet
        
        \see GREPacketType
     */
    struct Parse_GREPacket : public PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_BITFIELD         ( checksum_present,  1, bool );
        SENF_PARSER_PRIVATE_BITFIELD ( reserved0_,       12, unsigned ); // TODO: SKIP !!
        SENF_PARSER_BITFIELD_RO      ( version_number,    3, unsigned ); // TODO: Always Zero !!
        SENF_PARSER_FIELD            ( protocol_type,    Parse_UInt16 );
        SENF_PARSER_PRIVATE_VARIANT  ( checksum_,  checksum_present,
                                                   (VoidPacketParser) (Parse_UInt16) );
        SENF_PARSER_PRIVATE_VARIANT  ( reserved1_, checksum_present,
                                                   (VoidPacketParser) (Parse_UInt16) );

        SENF_PARSER_FINALIZE( Parse_GREPacket );

      private: 
        Parse_UInt16 checksum() const /// only defined if checksum_present() == \c true
             { return checksum_().get<1>(); }
    };
    
    /** \brief GRE packet
        
        \par Packet type (typedef):
            \ref GREPacket

        \par Fields:
            \ref Parse_GREPacket

        \ingroup protocolbundle_mpegdvb
     */
    struct GREPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<GREPacketType>
    {
        typedef PacketTypeMixin<GREPacketType> mixin;
        typedef ConcretePacket<GREPacketType> packet;
        typedef Parse_GREPacket parser;
    
        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;
        
        static void dump(packet p, std::ostream & os);
    };
    
    /** \brief GRE packet typedef */
    typedef GREPacketType::packet GREPacket;
    
  
}


///////////////////////////////hh.e////////////////////////////////////////
//#include "TransportPacket.cci"
//#include "TransportPacket.ct"
//#include "TransportPacket.cti"
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
