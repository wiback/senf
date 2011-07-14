// $Id$
//
// Copyright (C) 2009
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
//   Philipp Batroff <pug@berlios.de>

/** \file
    \brief IPv6Extension-Options public header */

#ifndef HH_SENF_Packets_DefaultBundle_IPv6ExtOptions_HH_
#define HH_SENF_Packets_DefaultBundle_IPv6ExtOptions_HH_ 1

// Custom includes
#include <senf/Packets/Packets.hh>

//#include "IPv6ExtOptions.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////
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


//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
