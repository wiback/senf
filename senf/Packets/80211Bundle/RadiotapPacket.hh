// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Christian Niephaus <cni@berlios.de>
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
    \brief Radiotap header */

#ifndef HH_SENF_Packets_80211Bundle_RadiotapPacket_
#define HH_SENF_Packets_80211Bundle_RadiotapPacket_ 1

// Custom includes
#include <senf/Packets/Packets.hh>
#include <boost/array.hpp>

///////////////////////////////hh.p////////////////////////////////////////
namespace senf {

    /** \brief Parse Flag field in Radiotap header

        <b>Re-ordering of bits due to LSB byte order</b>
     */
    struct RadiotapPacketParser_Flags : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_BITFIELD ( shortGI,        1, bool );
        SENF_PARSER_BITFIELD ( badFCS,         1, bool );
        SENF_PARSER_BITFIELD ( padding,        1, bool );
        SENF_PARSER_BITFIELD_RO ( fcsAtEnd,    1, bool ); // Cannot change this (change packet size)
        SENF_PARSER_BITFIELD ( fragmentation,  1, bool );
        SENF_PARSER_BITFIELD ( wep,            1, bool );
        SENF_PARSER_BITFIELD ( shortPreamble,  1, bool );
        SENF_PARSER_BITFIELD ( cfp,            1, bool );

        SENF_PARSER_FINALIZE ( RadiotapPacketParser_Flags );
    };

    /** \brief Parse in Radiotap Header channel frequency and flag field

        <b>Re-ordering of bits due to LSB byte order</b>
     */
    struct RadiotapPacketParser_ChannelOptions : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD     ( freq,          UInt16LSBParser );

        SENF_PARSER_BITFIELD  ( flag2ghz,             1, bool  );
        SENF_PARSER_BITFIELD  ( ofdm,                 1, bool  );
        SENF_PARSER_BITFIELD  ( cck,                  1, bool  );
        SENF_PARSER_BITFIELD  ( turbo,                1, bool  );
        SENF_PARSER_SKIP_BITS ( 4                              ); //currently unused in radiotap
        SENF_PARSER_BITFIELD  ( quarterRateChannel,   1, bool  );
        SENF_PARSER_BITFIELD  ( halfRateChannel,      1, bool  );
        SENF_PARSER_BITFIELD  ( gsm,                  1, bool  );
        SENF_PARSER_BITFIELD  ( staticTurbo,          1, bool  );
        SENF_PARSER_BITFIELD  ( gfsk,                 1, bool  );
        SENF_PARSER_BITFIELD  ( cckOfdm,              1, bool  );
        SENF_PARSER_BITFIELD  ( passive,              1, bool  );
        SENF_PARSER_BITFIELD  ( flag5ghz,             1, bool  );

        SENF_PARSER_FINALIZE ( RadiotapPacketParser_ChannelOptions );
    };

    /** \brief Parse an Radiotap header

        Parser implementing the Radiotap header

        Radiotap requires that all fields in the radiotap header
        are aligned to natural boundaries. For radiotap,
        that means all 8-, 16-, 32-, and 64-bit fields
        must begin on 8-, 16-, 32-, and 64-bit boundaries, respectively.
        In this way, generators and parsers can avoid unaligned
        accesses to radiotap capture fields. Radiotap-compliant
        generators must insert padding before a capture field
        to ensure its natural alignment.

        \see <a href="http://www.radiotap.org">Radiotap.org</a>

        \todo extended present field (bit 31 of present field is set)
    */
    struct RadiotapPacketParser_Header : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        /*
         * mandatory fields
         */
        SENF_PARSER_FIELD ( version, UInt8Parser     );
        //padding bits, currently unused, it simply aligns the fields onto natural word boundaries.
        SENF_PARSER_SKIP  ( 1                        );
        SENF_PARSER_FIELD ( length,  UInt16LSBParser );

        SENF_PARSER_PRIVATE_FIELD ( presentFlags, UInt32LSBParser );
        SENF_PARSER_GOTO( presentFlags );

        /*
         * present flags
         * indicate which data field are contained in the packet
         */
        SENF_PARSER_BITFIELD_RO ( lockQualityPresent,      1, bool );
        SENF_PARSER_BITFIELD_RO ( dbmAntennaNoisePresent,  1, bool );
        SENF_PARSER_BITFIELD_RO ( dbmAntennaSignalPresent, 1, bool );
        SENF_PARSER_BITFIELD_RO ( fhssPresent,             1, bool );
        SENF_PARSER_BITFIELD_RO ( channelOptionsPresent,   1, bool );
        SENF_PARSER_BITFIELD_RO ( ratePresent,             1, bool );
        SENF_PARSER_BITFIELD_RO ( flagsPresent,            1, bool );
        SENF_PARSER_BITFIELD_RO ( tsftPresent,             1, bool );
        SENF_PARSER_SKIP_BITS   ( 1                                ); //currently unused bits
        SENF_PARSER_BITFIELD_RO ( headerFcsPresent,        1, bool );
        SENF_PARSER_BITFIELD_RO ( dbAntennaNoisePresent,   1, bool );
        SENF_PARSER_BITFIELD_RO ( dbAntennaSignalPresent,  1, bool );
        SENF_PARSER_BITFIELD_RO ( antennaPresent,          1, bool );
        SENF_PARSER_BITFIELD_RO ( dbmTxAttenuationPresent, 1, bool );
        SENF_PARSER_BITFIELD_RO ( dbTxAttenuationPresent,  1, bool );
        SENF_PARSER_BITFIELD_RO ( txAttenuationPresent,    1, bool );
        SENF_PARSER_SKIP_BITS   ( 8                                ); //currently unused bits
        //if bit is set,another 32 bit present flag is attached (not implemented yet)
        SENF_PARSER_BITFIELD_RO ( extendedBitmaskPresent,  1, bool );
        SENF_PARSER_SKIP_BITS   ( 7                                ); //currently unused bits

        SENF_PARSER_FINALIZE ( RadiotapPacketParser_Header );
    };

    struct RadiotapPacketParser_FrameType : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_SKIP_BITS(4);
        SENF_PARSER_BITFIELD_RO( frameType, 2, unsigned );
        SENF_PARSER_SKIP_BITS(2);

        SENF_PARSER_FINALIZE(RadiotapPacketParser_FrameType);
    };

    struct RadiotapPacketParser : public RadiotapPacketParser_Header
    {
        RadiotapPacketParser(data_iterator i, state_type s) : RadiotapPacketParser_Header(i,s) {}

        static const size_type init_bytes = RadiotapPacketParser_Header::fixed_bytes;

        size_type bytes() const { return length(); }

        // ////////////////////////////////////////////////////////////////////////

        UInt64LSBParser            tsft()
            { return parseField<UInt64LSBParser>             (0); }
        RadiotapPacketParser_Flags flags()
            { return parseField<RadiotapPacketParser_Flags>  (1); }
        UInt8Parser                rate()
            { return parseField<UInt8Parser>                 (2); }
        RadiotapPacketParser_ChannelOptions channelOptions()
            { return parseField<RadiotapPacketParser_ChannelOptions>(3); }
        UInt16LSBParser            fhss()
            { return parseField<UInt16LSBParser>             (4); }
        Int8Parser                 dbmAntennaSignal()
            { return parseField<Int8Parser>                  (5); }
        Int8Parser                 dbmAntennaNoise()
            { return parseField<Int8Parser>                  (6); }
        UInt16LSBParser            lockQuality()
            { return parseField<UInt16LSBParser>             (7); }
        UInt16LSBParser            txAttenuation()
            { return parseField<UInt16LSBParser>             (8); }
        UInt16LSBParser            dbTxAttenuation()
            { return parseField<UInt16LSBParser>             (9); }
        Int8Parser                 dbmTxAttenuation()
            { return parseField<Int8Parser>                 (10); }
        UInt8Parser                antenna()
            { return parseField<UInt8Parser>                (11); }
        UInt8Parser                dbAntennaSignal()
            { return parseField<UInt8Parser>                (12); }
        UInt8Parser                dbAntennaNoise()
            { return parseField<UInt8Parser>                (13); }
        UInt32Parser               headerFcs()
            { return parseField<UInt32Parser>               (14); }

        unsigned frameType()
            { return parse<RadiotapPacketParser_FrameType>(length()).frameType(); }

        UInt32Parser fcs()
            { return parse<senf::UInt32Parser>(data().end()-4); }

    private:
        static const size_type fixed_bytes = 0; // 'remove' this member ...
        static const unsigned MAX_INDEX = 14;

        typedef boost::array<size_type,MAX_INDEX+2> OffsetTable;

        OffsetTable const & offsetTable(boost::uint32_t presentFlags);
        static void fillOffsetTable(boost::uint8_t * data, int maxLength, OffsetTable & table);

        template <class Parser>
        Parser parseField(unsigned index)
            { return parse<Parser>(offsetTable(presentFlags())[index]); }

        size_type calculateSize()
            { return offsetTable(presentFlags())[MAX_INDEX+1]; }

        friend class RadiotapPacketType;
    };

    /** \brief Radiotap packet

        \par Packet type (typedef):
            \ref RadiotapPacket

        \par Fields:
            \ref RadiotapPacketParser
            \image html RadiotapPacket.png

        \see http://www.radiotap.org/

        \ingroup protocolbundle_80211
     */
    struct RadiotapPacketType
        : public PacketTypeBase,
          public PacketTypeMixin<RadiotapPacketType>
    {
        typedef PacketTypeMixin<RadiotapPacketType> mixin;
        typedef ConcretePacket<RadiotapPacketType> packet;
        typedef RadiotapPacketParser parser;

        using mixin::init;
        using mixin::initSize;

        static void dump(packet p, std::ostream &os);
        static void finalize(packet p);
        static factory_t nextPacketType(packet p);
        static optional_range nextPacketRange(packet p);
    };

    typedef ConcretePacket<RadiotapPacketType> RadiotapPacket;
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "RadiotapPacket.cci"
//#include "RadiotapPacket.ct"
//#include "RadiotapPacket.cti"
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
