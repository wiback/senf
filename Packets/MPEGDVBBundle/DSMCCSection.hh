// $Id:DSMCCSection.hh 327 2007-07-20 10:03:44Z tho $
//
// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>
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
    \brief DSMCCSection public header */

#ifndef HH_DSMCCSection_
#define HH_DSMCCSection_ 1

// Custom includes
#include <algorithm>
#include "../../Packets/Packets.hh"

//#include "DSMCCSection.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Parse a DSMCC Section
    
        Parser implementing the header of a DSMCC Section
        
        \see DSMCCSectionType
     */
    struct Parse_DSMCCSection : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSE_FIELD( table_id, Parse_UInt8 );
        
        SENF_PARSE_BITFIELD( ssi,                  1, bool     );
        SENF_PARSE_BITFIELD( private_indicator,    1, bool     );
        SENF_PARSE_BITFIELD( reserved_1,           2, unsigned );
        SENF_PARSE_BITFIELD( sec_length,          12, unsigned );

        SENF_PARSE_FIELD( table_id_extension, Parse_UInt16 );

        SENF_PARSE_BITFIELD( reserved_2,           2, unsigned );
        SENF_PARSE_BITFIELD( version_num,          5, unsigned );
        SENF_PARSE_BITFIELD( curr_next_indicator,  1, bool     );

        SENF_PARSE_FIELD( sec_num,            Parse_UInt8  );
        SENF_PARSE_FIELD( last_sec_num,       Parse_UInt8  );
        
        SENF_PARSER_FINALIZE(Parse_DSMCCSection);

        Parse_UInt32 crc() const { return parse<Parse_UInt32>( data().size()-4 ); }
    };
    
    /** \brief DSMCC Section
        
        \par Packet type (typedef):
            \ref DSMCCSection

        \par Fields:
            \ref Parse_DSMCCSection

        \ingroup protocolbundle_mpegdvb
     */
    struct DSMCCSectionType
        : public PacketTypeBase,
          public PacketTypeMixin<DSMCCSectionType>
    {
        typedef PacketTypeMixin<DSMCCSectionType> mixin;
        typedef ConcretePacket<DSMCCSectionType> packet;
        typedef Parse_DSMCCSection parser;

        using mixin::nextPacketRange;
//          using mixin::nextPacketType;
        using mixin::init;
        
        static void dump(packet p, std::ostream & os);
        
        static PacketParserBase::size_type initSize();
        static PacketParserBase::size_type initHeadSize();
    };
        
    typedef DSMCCSectionType::packet DSMCCSection;
}


///////////////////////////////hh.e////////////////////////////////////////
//#include "DSMCCSection.cci"
//#include "DSMCCSection.ct"
//#include "DSMCCSection.cti"
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
