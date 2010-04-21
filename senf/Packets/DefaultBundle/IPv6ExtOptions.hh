// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Philipp.Batroff@fokus.fraunhofer.de
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
    \brief IPv6Extension-Options public header */

#ifndef HH_SENF_Packets_DefaultBundle_IPv6ExtOptions_HH_
#define HH_SENF_Packets_DefaultBundle_IPv6ExtOptions_HH_ 1

// Custom includes
#include <senf/Packets/Packets.hh>

//#include "IPv6ExtOptions.mpp"
///////////////////////////////hh.p////////////////////////////////////////
namespace senf {

    /** \brief IPv6 Extension Option parser

        Definition of parser fields in RFC2460 and assignments of <b>type</b> field at <a href="http://www.iana.org/assignments/ipv6-parameters">IANA</a>

        Used in \ref IPv6HopByHopOptionsPacketParser and  \ref IPv6DestinationOptionsPacketParser.

        \see <a href="http://www.iana.org/assignments/ipv6-parameters">IANA IPv6 Parameters</a>\n
             \ref IPv6GenericOptionParser

        \ingroup protocolbundle_default
     */

    class IPv6OptionParser : public PacketParserBase
    {
    public:
#       include SENF_PARSER()
        SENF_PARSER_FIELD    ( type,  UInt8Parser );
        SENF_PARSER_GOTO     ( type );
        SENF_PARSER_BITFIELD ( altAction,  2, unsigned );
        SENF_PARSER_BITFIELD ( changeFlag, 1, unsigned );
        SENF_PARSER_BITFIELD ( optionType, 5, unsigned );
        SENF_PARSER_FIELD    ( length, UInt8Parser );
        SENF_PARSER_FINALIZE ( IPv6OptionParser );
        
        typedef GenericTLVParserRegistry<IPv6OptionParser> Registry;
    };

    typedef GenericTLVParserBase<IPv6OptionParser> IPv6GenericOptionParser;

}


///////////////////////////////hh.e////////////////////////////////////////
//#include "IPv6ExtOptions.cci"
//#include "IPv6ExtOptions.ct"
//#include "IPv6ExtOptions.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
