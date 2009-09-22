// $Id: IPv6ExtOptionType.hh 869 2008-06-09 13:57:27Z pug $
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

#ifndef HH_SENF_Packets_DefaultBundle_IPv6ExtOptionType_HH_
#define HH_SENF_Packets_DefaultBundle_IPv6ExtOptionType_HH_ 1

// Custom includes
#include <senf/Packets/Packets.hh>

//#include "IPv6ExtOptionType.mpp"
///////////////////////////////hh.p////////////////////////////////////////
namespace senf {

    class OptTypeTLVPacketParser : public PacketParserBase 
    {
    public:
#       include SENF_PARSER()
        SENF_PARSER_BITFIELD (altAction,  2, unsigned);
        SENF_PARSER_BITFIELD (changeFlag, 1, unsigned);
        SENF_PARSER_BITFIELD (optionType, 5, unsigned);
        SENF_PARSER_FIELD (optionLength, UInt8Parser);
        SENF_PARSER_FINALIZE (OptTypeTLVPacketParser);
    };

    struct GenericOptTypeTLVPacketParser : public OptTypeTLVPacketParser 
    {
#       include SENF_PARSER()
        SENF_PARSER_INHERIT ( OptTypeTLVPacketParser );
        SENF_PARSER_SKIP ( optionLength(), 0 );
        SENF_PARSER_FINALIZE ( GenericOptTypeTLVPacketParser );

        senf::PacketInterpreterBase::range value() const;

        template<class ForwardReadableRange>
        void value(ForwardReadableRange const &range);
    };

}


///////////////////////////////hh.e////////////////////////////////////////
//#include "IPv6ExtOptionType.cci"
#include "IPv6ExtOptionType.ct"
//#include "IPv6ExtOptionType.cti"
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
