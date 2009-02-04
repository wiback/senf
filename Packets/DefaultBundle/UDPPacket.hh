// $Id$
//
// Copyright (C) 2006
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
    \brief UDPPacket public header */

#ifndef HH_SENF_Packets_DefaultBundle_UDPPacket_
#define HH_SENF_Packets_DefaultBundle_UDPPacket_ 1

// Custom includes
#include "../../Packets/Packets.hh"

//#include "UDPPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Parse a UDP packet

        Parser implementing the UDP header. The fields implemented are:
        \image html UDPPacket.png
        
        \see UDPPacketType \n
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

        boost::uint16_t calcChecksum() const; ///< calculate (pseudo-)header checksum
                                              /**< calculate and return the checksum of the
                                                   (pseudo-)header \see \ref senf::IpChecksum */
        bool validateChecksum() const {
            return checksum() == 0u || checksum() == calcChecksum();
        }                               ///< validate header checksum
                                        /**< return \c true if the \ref checksum() "checksum"
                                             field is equal to the \ref calcChecksum() "calculated checksum" */
    };

    /** \brief UDP packet
        
        \par Packet type (typedef):
            \ref UDPPacket

        \par Fields:
            \ref UDPPacketParser

        <table class="packet" cellpadding="5" cellspacing="1" border="1">
          <tr>
            <th width="25%">0</th> <th width="25%">8</th> <th width="25%">16</th>
            <th width="15%">24</th> <th style="text-align:right" width="10%">31</th>
          </tr><tr>
            <td colspan="2">\ref UDPPacketParser::source() "Source Port"</td>
            <td colspan="3">\ref UDPPacketParser::destination() "Destination Port"</td>
          </tr><tr>
            <td colspan="2">\ref UDPPacketParser::length() "Length"</td>
            <td colspan="3">\ref UDPPacketParser::checksum() "Checksum"</td>
          </tr>
        </table>
          
        \par Finalize action:
            \copydetails finalize()

        \ingroup protocolbundle_default
     */
    struct UDPPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<UDPPacketType>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<UDPPacketType> mixin;
#endif
        typedef ConcretePacket<UDPPacketType> packet; ///< UDP packet typedef
        typedef UDPPacketParser parser;               ///< typedef to the parser of UDP packet

        using mixin::nextPacketRange;
        using mixin::initSize;
        using mixin::init;

        /** \brief Dump given UDPPacket in readable form to given output stream */
        static void dump(packet p, std::ostream & os);
        
        static void finalize(packet p); ///< Finalize packet.
                                        /**< \li set \ref UDPPacketParser::length() "length" from 
                                               payload size
                                             \li calculate and set \ref UDPPacketParser::checksum() 
                                               "checksum" */
    };

    /** \brief UDP packet typedef */
    typedef ConcretePacket<UDPPacketType> UDPPacket;
}


///////////////////////////////hh.e////////////////////////////////////////
#endif
#ifndef SENF_PACKETS_DECL_ONLY
//#include "UDPPacket.cci"
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
