// $Id$
//
// Copyright (C) 2008
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
//   Stefan Bund <g0dil@berlios.de>
//   Christian Niephaus <cni@berlios.de>

/** \file
    \brief Radiotap header */

#ifndef HH_SENF_Packets_80211Bundle_RadiotapPacket_
#define HH_SENF_Packets_80211Bundle_RadiotapPacket_ 1

// Custom includes
#include <senf/Packets/Packets.hh>
#include <boost/array.hpp>

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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

        friend class RadiotapPacketParser;
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
        SENF_PARSER_SKIP_BITS ( 4                              );
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

    struct RadiotapPacketParser_RxFlags : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_SKIP_BITS ( 6                              );
        SENF_PARSER_BITFIELD  ( badPlcp,              1, bool  );
        SENF_PARSER_SKIP_BITS ( 1                              );

        SENF_PARSER_FINALIZE( RadiotapPacketParser_RxFlags );
    };

    struct RadiotapPacketParser_TxFlags : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_SKIP_BITS ( 5                              );
        SENF_PARSER_BITFIELD  ( txRts,                1, bool  );
        SENF_PARSER_BITFIELD  ( txCts,                1, bool  );
        SENF_PARSER_BITFIELD  ( fail,                 1, bool  );

        SENF_PARSER_FINALIZE( RadiotapPacketParser_TxFlags );
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

        SENF_PARSER_FINALIZE ( RadiotapPacketParser_Header );

        enum PresentIndex {
            // Could use the the entries from radiotap.h but I don't know,
            // if I want to pollute the global and macro namespace even more ...
            TSFT_INDEX              =  0,
            FLAGS_INDEX             =  1,
            RATE_INDEX              =  2,
            CHANNEL_INDEX           =  3,
            FHSS_INDEX              =  4,
            DBM_ANTSIGNAL_INDEX     =  5,
            DBM_ANTNOISE_INDEX      =  6,
            LOCK_QUALITY_INDEX      =  7,
            TX_ATTENUATION_INDEX    =  8,
            DB_TX_ATTENUATION_INDEX =  9,
            DBM_TX_POWER_INDEX      = 10,
            ANTENNA_INDEX           = 11,
            DB_ANTSIGNAL_INDEX      = 12,
            DB_ANTNOISE_INDEX       = 13,
            RX_FLAGS_INDEX          = 14,
            TX_FLAGS_INDEX          = 15,
            RTS_RETRIES_INDEX       = 16,
            DATA_RETRIES_INDEX      = 17,

            MAX_INDEX               = 17,

            RADIOTOP_NS_INDEX       = 29,
            VENDOR_NS_INDEX         = 30,
            EXTENDED_BITMASK_INDEX  = 31
        };

        enum PresentFlag {
            TSFT_FLAG               = (1<<TSFT_INDEX),
            FLAGS_FLAG              = (1<<FLAGS_INDEX),
            RATE_FLAG               = (1<<RATE_INDEX),
            CHANNEL_FLAG            = (1<<CHANNEL_INDEX),
            FHSS_FLAG               = (1<<FHSS_INDEX),
            DBM_ANTSIGNAL_FLAG      = (1<<DBM_ANTSIGNAL_INDEX),
            DBM_ANTNOISE_FLAG       = (1<<DBM_ANTNOISE_INDEX),
            LOCK_QUALITY_FLAG       = (1<<LOCK_QUALITY_INDEX),
            TX_ATTENUATION_FLAG     = (1<<TX_ATTENUATION_INDEX),
            DB_TX_ATTENUATION_FLAG  = (1<<DB_TX_ATTENUATION_INDEX),
            DBM_TX_POWER_FLAG       = (1<<DBM_TX_POWER_INDEX),
            ANTENNA_FLAG            = (1<<ANTENNA_INDEX),
            DB_ANTSIGNAL_FLAG       = (1<<DB_ANTSIGNAL_INDEX),
            DB_ANTNOISE_FLAG        = (1<<DB_ANTNOISE_INDEX),
            RX_FLAGS_FLAG           = (1<<RX_FLAGS_INDEX),
            TX_FLAGS_FLAG           = (1<<TX_FLAGS_INDEX),
            RTS_RETRIES_FLAG        = (1<<RTS_RETRIES_INDEX),
            DATA_RETRIES_FLAG       = (1<<DATA_RETRIES_INDEX),

            RADIOTOP_NS_FLAG        = (1<<RADIOTOP_NS_INDEX),
            VENDOR_NS_FLAG          = (1<<VENDOR_NS_INDEX),
            EXTENDED_BITMASK_FLAG   = (1<<EXTENDED_BITMASK_INDEX)
        };

        static unsigned const FIELD_SIZE[MAX_INDEX+2];
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
        RadiotapPacketParser(data_iterator i, state_type s);

        static const size_type init_bytes = RadiotapPacketParser_Header::fixed_bytes;

        size_type bytes() const;

        //-////////////////////////////////////////////////////////////////////////

#       define FIELD(name,type,index)                                   \
            typedef type name ## _t;                                    \
            type name() { return parseField<type>(index); }             \
            bool has_ ## name() { return currentTable()[index]; }       \
            bool name ## Present() { return has_ ## name(); }           \
            type init_ ## name() { initField(index); return name(); }   \
            void disable_ ## name() { disableField(index); }

        FIELD( tsft,              UInt64LSBParser,                      TSFT_INDEX              );

        // flags is special: disabling 'flags' must also disable the 'fcs' field
        typedef RadiotapPacketParser_Flags flags_t;
        flags_t flags() { return parseField<flags_t>(FLAGS_INDEX); }
        bool has_flags() { return currentTable()[FLAGS_INDEX]; }
        bool flagsPresent() { return has_flags(); }
        flags_t init_flags() { initField(FLAGS_INDEX); return flags(); }
        void disable_flags() { disable_fcs(); disableField(FLAGS_INDEX); }

        FIELD( rate,              UInt8Parser,                          RATE_INDEX              );
        FIELD( channelOptions,    RadiotapPacketParser_ChannelOptions,  CHANNEL_INDEX           );
        FIELD( fhss,              UInt16LSBParser,                      FHSS_INDEX              );
        FIELD( dbmAntennaSignal,  Int8Parser,                           DBM_ANTSIGNAL_INDEX     );
        FIELD( dbmAntennaNoise,   Int8Parser,                           DBM_ANTNOISE_INDEX      );
        FIELD( lockQuality,       UInt16LSBParser,                      LOCK_QUALITY_INDEX      );
        FIELD( txAttenuation,     UInt16LSBParser,                      TX_ATTENUATION_INDEX    );
        FIELD( dbTxAttenuation,   UInt16LSBParser,                      DB_TX_ATTENUATION_INDEX );
        FIELD( dbmTxAttenuation,  Int8Parser,                           DBM_TX_POWER_INDEX      );
        FIELD( antenna,           UInt8Parser,                          ANTENNA_INDEX           );
        FIELD( dbAntennaSignal,   UInt8Parser,                          DB_ANTSIGNAL_INDEX      );
        FIELD( dbAntennaNoise,    UInt8Parser,                          DB_ANTNOISE_INDEX       );
        FIELD( rxFlags,           RadiotapPacketParser_RxFlags,         RX_FLAGS_INDEX          );
        FIELD( txFlags,           RadiotapPacketParser_TxFlags,         TX_FLAGS_INDEX          );
        FIELD( rtsRetries,        UInt8Parser,                          RTS_RETRIES_INDEX       );
        FIELD( dataRetries,       UInt8Parser,                          DATA_RETRIES_INDEX      );

#       undef FIELD

        typedef UInt32Parser fcs_t;
        UInt32Parser fcs();
        bool has_fcs();
        UInt32Parser init_fcs();
        void disable_fcs();

        unsigned frameType();

    private:
        static const size_type fixed_bytes = 0; // hide this member, just in case

        typedef boost::array<size_type,MAX_INDEX+2> OffsetTable;

        //-////////////////////////////////////////////////////////////////////////
        // Offset table handling

        static OffsetTable & offsetTable(boost::uint32_t presentFlags);
        // Fills the offset table based on a packet
        static void parseOffsetTable(boost::uint8_t * data, int maxLength, OffsetTable & table);
        // Generate an offset table just from the present flags
        static void buildOffsetTable(boost::uint32_t presentFlags, OffsetTable & table);

        //-////////////////////////////////////////////////////////////////////////

        OffsetTable const & currentTable() const;
        OffsetTable const & getTable(boost::uint32_t presentFlags) const;

        template <class Parser>
        Parser parseField(unsigned index);
        void initField(unsigned index);
        void disableField(unsigned index);

        size_type calculateSize() const;

        void updatePresentFlags(boost::uint32_t flags);
        void insertRemoveBytes(unsigned from, unsigned to, int bytes);

        OffsetTable const * currentTable_;

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

        using mixin::initSize;

        static void init(packet p);
        static void dump(packet p, std::ostream & os);
        static factory_t nextPacketType(packet p);
        static optional_range nextPacketRange(packet const & p);
    };

    typedef ConcretePacket<RadiotapPacketType> RadiotapPacket;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "RadiotapPacket.cci"
//#include "RadiotapPacket.ct"
#include "RadiotapPacket.cti"
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
