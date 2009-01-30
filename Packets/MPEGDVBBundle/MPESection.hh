// $Id:MPESection.hh 560 2007-12-13 14:39:37Z tho $
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief MPESection public header */

#ifndef HH_SENF_Packets_MPEGDVBBundle_MPESection_
#define HH_SENF_Packets_MPEGDVBBundle_MPESection_ 1

// Custom includes
#include <boost/crc.hpp>
#include <algorithm>
#include "../../Packets/Packets.hh"

//#include "MPESection.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    struct MPERealTimeParametersParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_BITFIELD ( delta_t,                  12, unsigned );
        SENF_PARSER_BITFIELD ( table_boundary,           1,  bool     );
        SENF_PARSER_BITFIELD ( frame_boundary,           1,  bool     );
        SENF_PARSER_BITFIELD ( address,                  18, unsigned );

        SENF_PARSER_FINALIZE( MPERealTimeParametersParser );
    };


    /** \brief Parse a MPE Section

        Parser implementing the header of a MPE Section

        \see MPESectionType
     */
    class MPESectionParser : public PacketParserBase
    {
    public:
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD( table_id, UInt8Parser );

        SENF_PARSER_BITFIELD        ( section_syntax_indicator, 1,  bool     );
        SENF_PARSER_BITFIELD        ( private_indicator,        1,  bool     );
        SENF_PARSER_PRIVATE_BITFIELD( reserved_1,               2,  unsigned );
        SENF_PARSER_BITFIELD        ( section_length,           12, unsigned );

        SENF_PARSER_FIELD( mac_addr_6, UInt8Parser );
        SENF_PARSER_FIELD( mac_addr_5, UInt8Parser );

        SENF_PARSER_PRIVATE_BITFIELD( reserved_2,          2, unsigned );
        SENF_PARSER_BITFIELD        ( payload_scrmbl_ctrl, 2, unsigned );
        SENF_PARSER_BITFIELD        ( addr_scrmbl_ctrl,    2, unsigned );
        SENF_PARSER_BITFIELD        ( llc_snap_flag,       1, bool     );
        SENF_PARSER_BITFIELD        ( curr_next_indicator, 1, bool     );

        SENF_PARSER_FIELD( section_num,      UInt8Parser );
        SENF_PARSER_FIELD( last_section_num, UInt8Parser );

        SENF_PARSER_FIELD( real_time_parameters, MPERealTimeParametersParser );

        SENF_PARSER_FINALIZE( MPESectionParser );

        SENF_PARSER_PRIVATE_BITFIELD( ip_datagram_version, 4, unsigned );

        SENF_PARSER_INIT() {
            table_id() = 0x3e;
            section_syntax_indicator() = 1;
            private_indicator() = 0;
            reserved_1() = 3;
            reserved_2() = 3;
            payload_scrmbl_ctrl() = 0;
            addr_scrmbl_ctrl() = 0;
            curr_next_indicator() = 1;
        }

        UInt32Parser crc() const { return parse<UInt32Parser>( data().size()-4 ); }
        boost::uint32_t calcCrc() const;

        friend class MPESectionType;

    private:
        typedef boost::crc_optimal<32, 0x04C11DB7, 0xFFFFFFFF, 0, false, false> crc32_t;
    };


    /** \brief MPE Section

        \par Packet type (typedef):
            \ref MPESection

        \par Fields:
            \ref MPESectionParser
            \image html MPESection.png

        \ingroup protocolbundle_mpegdvb
     */
    struct MPESectionType
        : public PacketTypeBase,
          public PacketTypeMixin<MPESectionType>
    {
        typedef PacketTypeMixin<MPESectionType> mixin;
        typedef ConcretePacket<MPESectionType> packet;
        typedef MPESectionParser parser;

        using mixin::nextPacketRange;
        using mixin::init;

        static void dump(packet p, std::ostream & os);
        static void finalize(packet p);
        static factory_t nextPacketType(packet p);
        static PacketParserBase::size_type initSize();
        static PacketParserBase::size_type initHeadSize();
    };

    typedef ConcretePacket<MPESectionType> MPESection;
}


///////////////////////////////hh.e////////////////////////////////////////
//#include "MPESection.cci"
//#include "MPESection.ct"
//#include "MPESection.cti"
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
