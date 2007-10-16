// $Id:DatagramSection.hh 327 2007-07-20 10:03:44Z tho $
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

/** \brief
    \file DatagramSection public header */

#ifndef HH_DatagramSection_
#define HH_DatagramSection_ 1

// Custom includes
#include <algorithm>
#include "../../Packets/Packets.hh"
#include "DSMCCSection.hh"


//#include "DatagramSection.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Parse a Datagram Section
    
        Parser implementing the Datagram Section.
    
        \see DatagramSectionType
     */
    struct Parse_DatagramSection : public Parse_DSMCCSection
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_INHERIT(Parse_DSMCCSection);

        SENF_PARSE_FIELD( mac_addr_4, Parse_UInt8 );
        SENF_PARSE_FIELD( mac_addr_3, Parse_UInt8 );
        SENF_PARSE_FIELD( mac_addr_2, Parse_UInt8 );
        SENF_PARSE_FIELD( mac_addr_1, Parse_UInt8 );

        SENF_PARSER_FINALIZE(Parse_DatagramSection);

        typedef Parse_UIntField < 2,  4 > Parse_payload_scrmbl_ctrl;
        typedef Parse_UIntField < 4,  6 > Parse_addr_scrmbl_ctrl;
        typedef Parse_Flag      <     6 > Parse_llc_snap_flag;

        Parse_UInt8 mac_addr_6() const { return parse<Parse_UInt8>( 3 ); }
        Parse_UInt8 mac_addr_5() const { return parse<Parse_UInt8>( 4 ); }

        Parse_payload_scrmbl_ctrl payload_scrmbl_ctrl() const {
            return parse<Parse_payload_scrmbl_ctrl>( 5 );
        }

        Parse_addr_scrmbl_ctrl addr_scrmbl_ctrl() const {
            return parse<Parse_addr_scrmbl_ctrl>( 5 );
        }

        Parse_llc_snap_flag llc_snap_flag() const {
            return parse<Parse_llc_snap_flag>( 5 );
        }
    };

    /** \brief Datagram Section
        
        \par Packet type (typedef):
            \ref DatagramSection

        \par Fields:
            \ref Parse_DatagramSection

        \ingroup protocolbundle_mpegdvb
     */
    struct DatagramSectionType
        : public DSMCCSectionType,
          public PacketTypeMixin<DatagramSectionType>
    {
        typedef PacketTypeMixin<DatagramSectionType> mixin;
        typedef ConcretePacket<DatagramSectionType> packet;
        typedef Parse_DatagramSection parser;

        using mixin::nextPacketRange;
        using mixin::init;
        
        static void dump(packet p, std::ostream & os);
        
        static PacketParserBase::size_type initSize();
        static PacketParserBase::size_type initHeadSize();
    };
        
    typedef DatagramSectionType::packet DatagramSection;
}


///////////////////////////////hh.e////////////////////////////////////////
//#include "DatagramSection.cci"
//#include "DatagramSection.ct"
//#include "DatagramSection.cti"
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
