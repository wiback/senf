// $Id:$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Christian Niephaus <christian.niephaus@fokus.fraunhofer.de>
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
    \brief Radiotap header \n
        <b>Radiotap uses least significant bit byte order</b>


    */

#ifndef HH_RadiotapPacket_
#define HH_RadioPacket_ 1

#include "../../Packets/Packets.hh"

namespace senf
{

    /** \brief Parse Flag field in Radiotap header
     * <b>Re-ordering of bits due to LSB byte order</b>

     */
    struct RadiotapPacketParser_Flags : public senf::PacketParserBase
    {
    #   include SENF_FIXED_PARSER()

        SENF_PARSER_BITFIELD ( shortGI,         1, bool);
        SENF_PARSER_BITFIELD ( badFCS,          1, bool);
        SENF_PARSER_BITFIELD ( padding,         1, bool);
        SENF_PARSER_BITFIELD ( fcsPresent,      1, bool);
        SENF_PARSER_BITFIELD ( fragmentation,   1, bool);
        SENF_PARSER_BITFIELD ( wep,             1, bool);
        SENF_PARSER_BITFIELD ( shortPreamble,   1, bool);
        SENF_PARSER_BITFIELD ( cfp,             1, bool);

        SENF_PARSER_FINALIZE ( RadiotapPacketParser_Flags );

    };

    /** \brief Parse in Radiotap Header channel frequency and flag field

        <b>Re-ordering of bits due to LSB byte order</b>
     */
    struct RadiotapPacketParser_ChannelOptions : public senf::PacketParserBase
    {
    #   include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD       ( freq,          UInt16LSBParser );

        SENF_PARSER_BITFIELD    ( flag2ghz,             1, bool  );
        SENF_PARSER_BITFIELD    ( ofdm,                 1, bool  );
        SENF_PARSER_BITFIELD    ( cck,                  1, bool  );
        SENF_PARSER_BITFIELD    ( turbo,                1, bool  );
        SENF_PARSER_SKIP_BITS   ( 4                              ); //currently unused in radiotap
        SENF_PARSER_BITFIELD    ( quarterRateChannel,   1, bool  );
        SENF_PARSER_BITFIELD    ( halfRateChannel,      1, bool  );
        SENF_PARSER_BITFIELD    ( gsm,                  1, bool  );
        SENF_PARSER_BITFIELD    ( staticTurbo,          1, bool  );
        SENF_PARSER_BITFIELD    ( gfsk,                 1, bool  );
        SENF_PARSER_BITFIELD    ( cckOfdm,              1, bool  );
        SENF_PARSER_BITFIELD    ( passive,              1, bool  );
        SENF_PARSER_BITFIELD    ( flag5ghz,             1, bool  );

        SENF_PARSER_FINALIZE ( RadiotapPacketParser_ChannelOptions );
    };

    /** \brief Parse an Radiotap header

        Parser implementing the Radiotap header

        \see <a href="http://www.radiotap.org">Radiotap.org</a>

        \todo extended present field (bit 31 of present field is set)
    */
    struct RadiotapPacketParser : public senf::PacketParserBase
    {
    #   include SENF_PARSER()

        /*
         * mandatory fields
         */
        SENF_PARSER_FIELD    ( version,            UInt8Parser      );
        //padding bits, currently unused, it simply aligns the fields onto natural word boundaries.
        SENF_PARSER_SKIP     ( 1,1                                  );
        SENF_PARSER_FIELD    ( length,             UInt16LSBParser);

        /*
         * present flags
         * indicate which data field are contained in the packet
         */
        SENF_PARSER_BITFIELD_RO    ( lockQualityPresent,       1,    bool );
        SENF_PARSER_BITFIELD_RO    ( dbmAntennaNoisePresent,   1,    bool );
        SENF_PARSER_BITFIELD_RO    ( dbmAntennaSignalPresent,  1,    bool );
        SENF_PARSER_BITFIELD_RO    ( fhssPresent,              1,    bool );
        SENF_PARSER_BITFIELD_RO    ( channelPresent,           1,    bool );
        SENF_PARSER_BITFIELD_RO    ( ratePresent,              1,    bool );
        SENF_PARSER_BITFIELD_RO    ( flagsPresent,             1,    bool );
        SENF_PARSER_BITFIELD_RO    ( tsftPresent,              1,    bool );
        SENF_PARSER_SKIP_BITS   ( 2                                    ); //currently unused bits
        SENF_PARSER_BITFIELD_RO    ( dbAntennaNoisePresent,    1,    bool );
        SENF_PARSER_BITFIELD_RO    ( dbAntennaSignalPresent,   1,    bool );
        SENF_PARSER_BITFIELD_RO    ( antennaPresent,           1,    bool );
        SENF_PARSER_BITFIELD_RO    ( dbmTxAttenuationPresent,  1,    bool );
        SENF_PARSER_BITFIELD_RO    ( dbTxAttenuationPresent,   1,    bool );
        SENF_PARSER_BITFIELD_RO    ( txAttenuationPresent,     1,    bool );
        SENF_PARSER_SKIP_BITS   ( 8                                    ); //currently unused bits
        //if bit is set,another 32 bit present flag is attached (not implemented yet)
        SENF_PARSER_BITFIELD    ( extendedBitmaskPresent,   1,    bool );
        SENF_PARSER_SKIP_BITS   ( 7                                    ); //currently unused bits


        /*
         * Radiotap data
         * parsing data according to present flags
         */
        SENF_PARSER_VARIANT ( tsft_,             tsftPresent,
                                ( novalue ( disable_tsft,               VoidPacketParser                    ))
                                ( id      ( tsft,                       UInt64LSBParser                     )) );
        SENF_PARSER_VARIANT ( flags_,            flagsPresent,
                                ( novalue ( disable_flags,              VoidPacketParser                    ))
                                ( id      ( flags,                      RadiotapPacketParser_Flags          )) );
        SENF_PARSER_VARIANT ( rate_,             ratePresent,
                                ( novalue ( disable_rate,               VoidPacketParser                    ))
                                ( id      ( rate,                       UInt8Parser                         )) );
        SENF_PARSER_VARIANT ( channelFlags_,     channelPresent,
                                ( novalue ( disable_channelOptions,       VoidPacketParser                    ))
                                ( id      ( channelOptions,               RadiotapPacketParser_ChannelOptions   )) );
        SENF_PARSER_VARIANT ( fhss_,             fhssPresent,
                                ( novalue ( disable_fhss,               VoidPacketParser                    ))
                                ( id      ( fhss,                       UInt16LSBParser                     )) );
        SENF_PARSER_VARIANT ( dbmAntennaSignal_, dbmAntennaSignalPresent,
                                ( novalue ( disable_dbmAntennaSignal,   VoidPacketParser                    ))
                                ( id      ( dbmAntennaSignal,           Int8Parser                          )) );
        SENF_PARSER_VARIANT ( dbmAntennaNoise_,  dbmAntennaNoisePresent,
                                ( novalue ( disable_dbmAntennaNoise,    VoidPacketParser                    ))
                                ( id      ( dbmAntennaNoise,            Int8Parser                          )) );
        SENF_PARSER_VARIANT ( lockQuality_,      lockQualityPresent,
                                ( novalue ( disable_lockQuality,        VoidPacketParser                    ))
                                ( id      ( lockQuality,                UInt16LSBParser                     )) );
        SENF_PARSER_VARIANT ( txAttenuation_,    txAttenuationPresent,
                                ( novalue ( disable_txAttenuation,      VoidPacketParser                    ))
                                ( id      ( txAttenuation,              UInt16LSBParser                     )) );
        SENF_PARSER_VARIANT ( dbTxAttenuation_,  dbTxAttenuationPresent,
                                ( novalue ( disable_dbTxAttenuation,    VoidPacketParser                    ))
                                ( id      ( dbTxAttenuation,            UInt16LSBParser                     )) );
        SENF_PARSER_VARIANT ( antenna_,          antennaPresent,
                                ( novalue ( disable_antenna,            VoidPacketParser                    ))
                                ( id      ( antenna,                    UInt8Parser                         )) );
        SENF_PARSER_VARIANT ( dbAntennaSignal_,  dbAntennaSignalPresent,
                                ( novalue ( disable_dbAntennaSignal,    VoidPacketParser                    ))
                                ( id      ( dbAntennaSignal,            UInt8Parser                         )) );
        SENF_PARSER_VARIANT ( dbAntennaNoise_, dbAntennaNoisePresent,
                                ( novalue ( disable_dbAntennaNoise,     VoidPacketParser                    ))
                                ( id      ( dbAntennaNoise,             UInt8Parser                         )) );

        SENF_PARSER_INIT() {
            version() = 0;
        }


        SENF_PARSER_FINALIZE(RadiotapPacketParser);
    };



    struct RadiotapPacketType
        : public senf::PacketTypeBase,
          public senf::PacketTypeMixin<RadiotapPacketType>
    {
        typedef senf::PacketTypeMixin<RadiotapPacketType> mixin;
        typedef senf::ConcretePacket<RadiotapPacketType> packet;
        typedef senf::RadiotapPacketParser parser;

        using mixin::nextPacketRange;
        using mixin::init;
        using mixin::initSize;

        static void dump(packet p, std::ostream &os);
        static void finalize(packet p);


    };

        typedef RadiotapPacketType::packet RadiotapPacket;
}

#endif
