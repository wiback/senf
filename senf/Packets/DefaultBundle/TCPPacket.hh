// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//	   Dennis Goslar <dennis.goslar@inf.hochschule-bonn-rhein-sieg.de>
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
    \brief TCPPacket public header */

#ifndef HH_SENF_Packets_DefaultBundle_TCPPacket_
#define HH_SENF_Packets_DefaultBundle_TCPPacket_ 1

// Custom includes
#include <senf/Packets/Packets.hh>

//#include "TCPPacket.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Parse a TCP packet

        Parser implementing the TCP header. The fields implemented are:
        \image html TCPPacket.png

        \see TCPPacketType \n
            <a href="http://tools.ietf.org/html/rfc793">RFC 793</a>
     */
    struct TCPPacketParser : public PacketParserBase
    {
#       include SENF_PARSER()

    	SENF_PARSER_FIELD( source,      			UInt16Parser );
    	SENF_PARSER_FIELD( destination,     		UInt16Parser );
    	SENF_PARSER_FIELD( sequencenumber,  		UInt32Parser );
    	SENF_PARSER_FIELD( acknowledgmentnumber,	UInt32Parser );

    	SENF_PARSER_BITFIELD        ( dataoffset, 4, unsigned );
    	SENF_PARSER_PRIVATE_BITFIELD( reserved,	  6, unsigned );
    	SENF_PARSER_BITFIELD        ( urgf,       1, bool     );
        SENF_PARSER_BITFIELD        ( ackf,       1, bool     );
        SENF_PARSER_BITFIELD        ( pshf,       1, bool     );
        SENF_PARSER_BITFIELD        ( rstf,       1, bool     );
        SENF_PARSER_BITFIELD        ( synf,       1, bool     );
        SENF_PARSER_BITFIELD        ( finf,       1, bool     );

        SENF_PARSER_FIELD( window,        UInt16Parser );
        SENF_PARSER_FIELD( checksum,      UInt16Parser );
        SENF_PARSER_FIELD( urgentpointer, UInt16Parser );

        // skip option part in TCP-Header, dataoffset()*4 = TCP header length in byte
        // for options see http://www.iana.org/assignments/tcp-parameters/
        // if dataoffset() < 5 packet is invalid
        SENF_PARSER_SKIP( (dataoffset() < 5 ? 0 : dataoffset()*4-20), 0);

        SENF_PARSER_FINALIZE(TCPPacketParser);

        SENF_PARSER_INIT() {
            // Reserved: 6 bits, reserved for future use. Must be zero.
            // see http://tools.ietf.org/html/rfc793
            reserved_() = 0;
            // dataoffset per default 5, 5*4 = 20 Byte Header Length
            dataoffset() = 5;
        }

        boost::uint16_t calcChecksum() const; ///< calculate (pseudo-)header checksum
                                              /**< calculate and return the checksum of the
                                                   (pseudo-)header \see \ref senf::IpChecksum */

        bool validateChecksum() const {
            return checksum() == 0u || checksum() == calcChecksum();
        }                               ///< validate header checksum
                                        /**< return \c true if the \ref checksum() "checksum"
                                             field is equal to the \ref calcChecksum() "calculated checksum" */
    };

    /** \brief TCP packet

        \par Packet type (typedef):
            \ref TCPPacket

        \par Fields:
            \ref TCPPacketParser

        <table class="packet" cellpadding="5" cellspacing="1" border="1">
        <tr>
            <th width="12%">0</th>
            <th width="20%">4</th>
            <th width="3%">10</th>
            <th width="3%">11</th>
            <th width="3%">12</th>
            <th width="3%">13</th>
            <th width="3%">14</th>
            <th width="3%">15</th>
            <th width="25%">16</th>
            <th style="text-align:right" width="25%">31</th>
        </tr>
        <tr>
            <td colspan="8">\ref TCPPacketParser::source() "Source Port"</td>
            <td colspan="2">\ref TCPPacketParser::destination() "Destination Port"</td>
        </tr><tr>
            <td colspan="10">\ref TCPPacketParser::sequencenumber() "Sequence Number"</td>
        </tr><tr>
            <td colspan="10">\ref TCPPacketParser::acknowledgmentnumber() "Acknowledgment Number"</td>
        </tr><tr>
            <td colspan="1">\ref TCPPacketParser::dataoffset() "DataOffset"</td>
            <td colspan="1">Reserved</td>
            <td colspan="1">\ref TCPPacketParser::urgf() "UR"</td>
            <td colspan="1">\ref TCPPacketParser::ackf() "AC"</td>
            <td colspan="1">\ref TCPPacketParser::pshf() "PS"</td>
            <td colspan="1">\ref TCPPacketParser::rstf() "RS"</td>
            <td colspan="1">\ref TCPPacketParser::synf() "SY"</td>
            <td colspan="1">\ref TCPPacketParser::finf() "FI"</td>
            <td colspan="2">\ref TCPPacketParser::window() "Window Size"</td>
        </tr><tr>
            <td colspan="8">\ref TCPPacketParser::checksum() "Checksum"</td>
            <td colspan="2">\ref TCPPacketParser::urgentpointer() "Urgent Pointer"</td>
        </tr>
        </table>

        \par Finalize action:
            \copydetails finalize()

        \ingroup protocolbundle_default
     */
    struct TCPPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<TCPPacketType>
    {
#ifndef DOXYGEN
        typedef PacketTypeMixin<TCPPacketType> mixin;
#endif
        typedef ConcretePacket<TCPPacketType> packet; ///< TCP packet typedef
        typedef TCPPacketParser parser;               ///< typedef to the parser of TCP packet

        using mixin::nextPacketRange;
        using mixin::initSize;
        using mixin::init;

        /** \brief Dump given TCPPacket in readable form to given output stream */
        static void dump(packet p, std::ostream & os);

        static void finalize(packet p); ///< Finalize packet.
                                        /**< \li set \ref TCPPacketParser::dataoffset() "Data Offset" from
                                               header size
                                             \li calculate and set \ref TCPPacketParser::checksum()
                                               "checksum" */
    };

    /** \brief TCP packet typedef */
    typedef ConcretePacket<TCPPacketType> TCPPacket;
}


///////////////////////////////hh.e////////////////////////////////////////
//#include "TCPPacket.cci"
//#include "TCPPacket.ct"
//#include "TCPPacket.cti"
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
