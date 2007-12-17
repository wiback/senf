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

/** \file
    \brief UDPPacket public header */

#ifndef HH_UDPPacket_
#define HH_UDPPacket_ 1

// Custom includes
#include "../../Packets/Packets.hh"

//#include "UDPPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Parse a UDP packet

        Parser implementing the UDP header. The fields implemented are:

        \see UDPPacketType
            <a href="http://tools.ietf.org/html/rfc768">RFC 768</a>
     */
    struct UDPPacketParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD( source,      senf::UInt16Parser );
        SENF_PARSER_FIELD( destination, senf::UInt16Parser );
        SENF_PARSER_FIELD( length,      senf::UInt16Parser );
        SENF_PARSER_FIELD( checksum,    senf::UInt16Parser );

        SENF_PARSER_FINALIZE(UDPPacketParser);

        boost::uint16_t calcChecksum() const;

        bool validateChecksum() const {
            return checksum() == 0u || checksum() == calcChecksum();
        }
    };

    /** \brief UDP packet
        
        \par Packet type (typedef):
            \ref UDPPacket

        \par Fields:
            \ref UDPPacketParser

        \par Finalize action:
            Set \a length from payload size\n
            Calculate \a checksum

        \ingroup protocolbundle_default
     */
    struct UDPPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<UDPPacketType>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<UDPPacketType> mixin;
        typedef ConcretePacket<UDPPacketType> packet;
        typedef UDPPacketParser parser;
#endif
        using mixin::nextPacketRange;
        using mixin::initSize;
        using mixin::init;

        static void dump(packet p, std::ostream & os);

        static void finalize(packet p);
    };

    /** \brief UDP packet typedef */
    typedef ConcretePacket<UDPPacketType> UDPPacket;
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
