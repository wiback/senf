// $Id$
//
// Copyright (C) 2006
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

/** \file
    \brief UDPPacket public header */

#ifndef HH_SENF_Packets_DefaultBundle_UDPPacket_
#define HH_SENF_Packets_DefaultBundle_UDPPacket_ 1

// Custom includes
#include <senf/Packets/Packets.hh>
#include "Registries.hh"

//-/////////////////////////////////////////////////////////////////////////////////////////////////

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
        typedef PacketTypeMixin<UDPPacketType> mixin;
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
        static const IPTypes::key_t ipType = 17;
    };

    /** \brief UDP packet typedef
        \ingroup protocolbundle_default
     */
    typedef ConcretePacket<UDPPacketType> UDPPacket;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( UDPPacket );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
