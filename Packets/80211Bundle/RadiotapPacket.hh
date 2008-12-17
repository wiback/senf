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

#include "../../Packets/Packets.hh"

namespace senf
{

    /** \brief Parse Flag field in Radiotap header

        <b>Re-ordering of bits due to LSB byte order</b>
     */
    struct RadiotapPacketParser_Flags : public PacketParserBase
    {
    #   include SENF_FIXED_PARSER()

        SENF_PARSER_BITFIELD ( shortGI,        1, bool );
        SENF_PARSER_BITFIELD ( badFCS,         1, bool );
        SENF_PARSER_BITFIELD ( padding,        1, bool );
        SENF_PARSER_BITFIELD ( fcsPresent,     1, bool );
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
    #   include SENF_FIXED_PARSER()

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
    struct RadiotapPacketParser : public PacketParserBase
    {
    #   include SENF_PARSER()

        /*
         * mandatory fields
         */
        SENF_PARSER_FIELD ( version, UInt8Parser     );
        //padding bits, currently unused, it simply aligns the fields onto natural word boundaries.
        SENF_PARSER_SKIP  ( 1,1                      );
        SENF_PARSER_FIELD ( length,  UInt16LSBParser );

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
        SENF_PARSER_BITFIELD_RO ( fcsPresent,              1, bool );
        SENF_PARSER_BITFIELD_RO ( dbAntennaNoisePresent,   1, bool );
        SENF_PARSER_BITFIELD_RO ( dbAntennaSignalPresent,  1, bool );
        SENF_PARSER_BITFIELD_RO ( antennaPresent,          1, bool );
        SENF_PARSER_BITFIELD_RO ( dbmTxAttenuationPresent, 1, bool );
        SENF_PARSER_BITFIELD_RO ( dbTxAttenuationPresent,  1, bool );
        SENF_PARSER_BITFIELD_RO ( txAttenuationPresent,    1, bool );
        SENF_PARSER_SKIP_BITS   ( 8                                ); //currently unused bits
        //if bit is set,another 32 bit present flag is attached (not implemented yet)
        SENF_PARSER_BITFIELD    ( extendedBitmaskPresent,  1, bool );
        SENF_PARSER_SKIP_BITS   ( 7                                ); //currently unused bits

        /*
         * Radiotap data
         * parsing data according to present flags
         *
         * PARSER_SKIP required to skip correct length of padding bits
         */

        /* macro to create required variant parser */
        #define OPTIONAL_FIELD(name, parser) SENF_PARSER_VARIANT \
            ( name##_, name##Present, \
              ( novalue( disable_##name, VoidPacketParser )) \
              (      id( name,           parser           )) );

        /* macro to create padding parser */
        #define SKIP_OPTIONAL_PADDING(cond, parser, size) \
            SENF_PARSER_SKIP( \
                    (cond ? (size - (parser##__offset() + \
                            senf::bytes(parser##_())) % size) % size : 0) , 0  );

        OPTIONAL_FIELD        ( tsft,                     UInt64LSBParser                     );
        OPTIONAL_FIELD        ( flags,                    RadiotapPacketParser_Flags          );
        OPTIONAL_FIELD        ( rate,                     UInt8Parser                         );
        SKIP_OPTIONAL_PADDING ( channelOptionsPresent(),  rate, 2                             );
        OPTIONAL_FIELD        ( channelOptions,           RadiotapPacketParser_ChannelOptions );
        SKIP_OPTIONAL_PADDING ( fhssPresent(),            channelOptions, 2                   );
        OPTIONAL_FIELD        ( fhss,                     UInt16LSBParser                     );
        OPTIONAL_FIELD        ( dbmAntennaSignal,         Int8Parser                          );
        OPTIONAL_FIELD        ( dbmAntennaNoise,          Int8Parser                          );
        SKIP_OPTIONAL_PADDING ( lockQualityPresent(),     dbmAntennaNoise, 2                  );
        OPTIONAL_FIELD        ( lockQuality,              UInt16LSBParser                     );
        SKIP_OPTIONAL_PADDING ( txAttenuationPresent(),   lockQuality, 2                      );
        OPTIONAL_FIELD        ( txAttenuation,            UInt16LSBParser                     );
        SKIP_OPTIONAL_PADDING ( dbTxAttenuationPresent(), txAttenuation, 2                    );
        OPTIONAL_FIELD        ( dbTxAttenuation,          UInt16LSBParser                     );
        OPTIONAL_FIELD        ( dbmTxAttenuation,         Int8Parser                          );
        OPTIONAL_FIELD        ( antenna,                  UInt8Parser                         );
        OPTIONAL_FIELD        ( dbAntennaSignal,          UInt8Parser                         );
        OPTIONAL_FIELD        ( dbAntennaNoise,           UInt8Parser                         );
        SKIP_OPTIONAL_PADDING ( fcsPresent(),             dbAntennaNoise, 4                   );
        OPTIONAL_FIELD        ( fcs,                      UInt32Parser                        );

        SENF_PARSER_INIT() {
            version() = 0;
        }

        SENF_PARSER_FINALIZE( RadiotapPacketParser );
    };

    /** \brief Radiotap packet

        \par Packet type (typedef):
            \ref RadiotapPacket

        \par Fields:
            \ref RadiotapPacketParser
        
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

        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;

        static void dump(packet p, std::ostream &os);
        static void finalize(packet p);
        static factory_t nextPacketType(packet p);
    };

    typedef ConcretePacket<RadiotapPacketType> RadiotapPacket;
}

#endif
