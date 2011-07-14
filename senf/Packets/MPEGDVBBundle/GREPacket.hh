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
//   Joachim Kaeber <jkaeber@berlios.de>
/** \file
    \brief GREPacket public header */

#ifndef HH_SENF_Packets_MPEGDVBBundle_GREPacket_
#define HH_SENF_Packets_MPEGDVBBundle_GREPacket_ 1

// Custom includes
#include <senf/Packets/DefaultBundle/EthernetPacket.hh>

//#include "GREPacket.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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
            \image html GREPacket.png

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


//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
