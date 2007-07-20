// $Id:DSMCCSection.hh 327 2007-07-20 10:03:44Z tho $
//
// Copyright (C) 2007
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

#ifndef HH_DSMCCSection_
#define HH_DSMCCSection_ 1

// Custom includes
#include <algorithm>
#include "Packets/PacketType.hh"
#include "Packets/ParseInt.hh"
#include "Packets/PacketRegistry.hh"
#include "Packets/PacketParser.hh"

//#include "DSMCCSection.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    ///\addtogroup protocolbundle_mpegdvb
    ///@{

    struct Parse_DSMCCSection : public PacketParserBase
    {
        SENF_PACKET_PARSER_INIT(Parse_DSMCCSection);

        ///////////////////////////////////////////////////////////////////////////

        typedef Parse_UInt8               Parse_table_id;
        typedef Parse_Flag      <     0 > Parse_ssi;  // section_syntax_indicator
        typedef Parse_Flag      <     1 > Parse_pi;   // private_indicator
        typedef Parse_UIntField < 2,  4 > Parse_reserved_1;
        typedef Parse_UIntField < 4, 16 > Parse_sec_length;
        typedef Parse_UIntField < 0,  2 > Parse_reserved_2;
        typedef Parse_UIntField < 2,  7 > Parse_version_num;
        typedef Parse_Flag      <     7 > Parse_curr_next_indicator;
                
        SENF_PACKET_PARSER_DEFINE_FIXED_FIELDS(
            ((Field       ) ( table_id,            Parse_table_id            )) 
            ((OverlayField) ( ssi,                 Parse_ssi                 ))
            ((OverlayField) ( pi,                  Parse_pi                  ))
            ((OverlayField) ( reserved_1,          Parse_reserved_1          ))
            ((Field       ) ( sec_length,          Parse_sec_length          ))
            ((Field       ) ( tabel_id_extension,  Parse_UInt16              ))
            ((OverlayField) ( reserved_2,          Parse_reserved_2          ))
            ((OverlayField) ( version_num,         Parse_version_num         ))
            ((Field       ) ( curr_next_indicator, Parse_curr_next_indicator ))
            ((Field       ) ( sec_num,             Parse_UInt8               ))
            ((Field       ) ( last_sec_num,        Parse_UInt8               ))
        );
        
        Parse_UInt32 crc() const { return parse<Parse_UInt32>( data().size()-4 ); }
    };

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
    
    ///@}
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
