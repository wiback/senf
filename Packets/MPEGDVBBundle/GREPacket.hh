// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Joachim Kaeber <jkaeber@berlios.de>
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
#include "../DefaultBundle/EthernetPacket.hh"

//#include "GREPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Parse a GRE packet

        Parser implementing the header of a General Routing Encapsulation (GRE, RFC 2784) Packet
        
        \see GREPacketType
     */
    struct GREChecksumParser : public PacketParserBase {
#       include SENF_PARSER()        
        SENF_PARSER_FIELD ( checksum1_, UInt16Parser );
        SENF_PARSER_PRIVATE_FIELD ( reserved1_, UInt16Parser );
        SENF_PARSER_FINALIZE(GREChecksumParser);
    };

    struct GREPacketParser : public PacketParserBase
    {
#       include SENF_PARSER()

        SENF_PARSER_BITFIELD         ( checksum_present,  1, bool );
        SENF_PARSER_PRIVATE_BITFIELD ( reserved0_,       12, unsigned ); // TODO: SKIP !!
        SENF_PARSER_BITFIELD         ( version_number,    3, unsigned ); // TODO: Always Zero !!
        SENF_PARSER_FIELD            ( protocol_type,    UInt16Parser );
        SENF_PARSER_PRIVATE_VARIANT  ( checksum_,  checksum_present,
                                                   (VoidPacketParser) (GREChecksumParser) );
 
        SENF_PARSER_FINALIZE( GREPacketParser );

      private: 
        UInt16Parser checksum() const /// only defined if checksum_present() == \c true
             { return checksum_().get<1>().checksum1_(); }
    };
    
    /** \brief GRE packet
        
        \par Packet type (typedef):
            \ref GREPacket

        \par Fields:
            \ref GREPacketParser

        \ingroup protocolbundle_mpegdvb
     */
    struct GREPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<GREPacketType, EtherTypes>
    {
        typedef PacketTypeMixin<GREPacketType, EtherTypes> mixin;
        typedef ConcretePacket<GREPacketType> packet;
        typedef GREPacketParser parser;
    
        using mixin::nextPacketRange;
        using mixin::nextPacketType;
        using mixin::init;
        using mixin::initSize;
        
        static void dump(packet p, std::ostream & os);
        static EtherTypes::key_t nextPacketKey(packet p) {
          return p->protocol_type();
        }
        static void finalize(packet p) {
          p->protocol_type() << key(p.next(nothrow));
          p->version_number() = 0; // as per RFC2784, 2.3.1

          if (p->checksum_present()) { 
            // compute checksum
          } else {
            // ???
          }
        }
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
