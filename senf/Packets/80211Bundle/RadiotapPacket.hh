//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief Radiotap header */

#ifndef HH_SENF_Packets_80211Bundle_RadiotapPacket_
#define HH_SENF_Packets_80211Bundle_RadiotapPacket_ 1

// Custom includes
#include <senf/Packets/Packets.hh>
#include <boost/unordered_map.hpp>
#include <boost/array.hpp>
#include <boost/functional/hash.hpp>

//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace senf {

    /** \brief Parse Flag field in Radiotap header

        <b>Re-ordering of bits due to LSB byte order</b>
     */
    struct RadiotapPacket_FlagsParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_BITFIELD   ( shortGI,       1, bool );
        SENF_PARSER_BITFIELD   ( badFCS,        1, bool );
        SENF_PARSER_BITFIELD   ( padding,       1, bool );
        SENF_PARSER_BITFIELD_RO( fcsAtEnd,      1, bool ); // Cannot change this (change packet size)
        SENF_PARSER_BITFIELD   ( fragmentation, 1, bool );
        SENF_PARSER_BITFIELD   ( wep,           1, bool );
        SENF_PARSER_BITFIELD   ( shortPreamble, 1, bool );
        SENF_PARSER_BITFIELD   ( cfp,           1, bool );

        SENF_PARSER_FINALIZE( RadiotapPacket_FlagsParser );

        friend struct RadiotapPacketParser;
    };

    /** \brief Parse in Radiotap Header channel frequency and flag field

        <b>Re-ordering of bits due to LSB byte order</b>
     */
    struct RadiotapPacket_ChannelOptionsParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD    ( freq,       UInt16LSBParser );
        SENF_PARSER_BITFIELD ( flag2ghz,           1, bool );
        SENF_PARSER_BITFIELD ( ofdm,               1, bool );
        SENF_PARSER_BITFIELD ( cck,                1, bool );
        SENF_PARSER_BITFIELD ( turbo,              1, bool );
        SENF_PARSER_SKIP_BITS(                     4       );
        SENF_PARSER_BITFIELD ( quarterRateChannel, 1, bool );
        SENF_PARSER_BITFIELD ( halfRateChannel,    1, bool );
        SENF_PARSER_BITFIELD ( gsm,                1, bool );
        SENF_PARSER_BITFIELD ( staticTurbo,        1, bool );
        SENF_PARSER_BITFIELD ( gfsk,               1, bool );
        SENF_PARSER_BITFIELD ( cckOfdm,            1, bool );
        SENF_PARSER_BITFIELD ( passive,            1, bool );
        SENF_PARSER_BITFIELD ( flag5ghz,           1, bool );

        SENF_PARSER_FINALIZE( RadiotapPacket_ChannelOptionsParser );
    };

    struct RadiotapPacket_RxFlagsParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_SKIP_BITS(         14       );
        SENF_PARSER_BITFIELD ( badPlcp, 1, bool );
        SENF_PARSER_SKIP_BITS(          1       );

        SENF_PARSER_FINALIZE( RadiotapPacket_RxFlagsParser );
    };

    struct RadiotapPacket_TxFlagsParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_SKIP_BITS(       11       );
        SENF_PARSER_BITFIELD ( noSeq, 1, bool );
        SENF_PARSER_BITFIELD ( noAck, 1, bool );
        SENF_PARSER_BITFIELD ( txRts, 1, bool );
        SENF_PARSER_BITFIELD ( txCts, 1, bool );
        SENF_PARSER_BITFIELD ( fail,  1, bool );

        SENF_PARSER_FINALIZE( RadiotapPacket_TxFlagsParser );
    };

    struct RadiotapPacket_MCSParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_SKIP_BITS(                     3           );
        SENF_PARSER_BITFIELD ( fecTypeKnown,       1, bool     );
        SENF_PARSER_BITFIELD ( htFormatKnown,      1, bool     );
        SENF_PARSER_BITFIELD ( guardIntervalKnown, 1, bool     );
        SENF_PARSER_BITFIELD ( mcsIndexKnown,      1, bool     );
        SENF_PARSER_BITFIELD ( bandwidthKnown,     1, bool     );
        SENF_PARSER_SKIP_BITS(                     3           );
        SENF_PARSER_BITFIELD ( fecType,            1, bool     );
        SENF_PARSER_BITFIELD ( htFormat,           1, bool     );
        SENF_PARSER_BITFIELD ( guardInterval,      1, bool     );
        SENF_PARSER_BITFIELD ( bandwidth,          2, unsigned );

        SENF_PARSER_FIELD    ( mcsIndex,           UInt8Parser );

        SENF_PARSER_FINALIZE( RadiotapPacket_MCSParser );
    };

    struct RadiotapPacket_VHTParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_BITFIELD ( groupIdKnown,                   1, bool     );
        SENF_PARSER_BITFIELD ( bandwidthKnown,                 1, bool     );
        SENF_PARSER_BITFIELD ( beamformedKnown,                1, bool     );
        SENF_PARSER_BITFIELD ( ldpcExtraOfdmSymbolKnown,       1, bool     ); 
        SENF_PARSER_BITFIELD ( shortGiNsymDisambiguationKnown, 1, bool     );
        SENF_PARSER_BITFIELD ( guardIntervalKnown,             1, bool     );
        SENF_PARSER_BITFIELD ( txOpPsNotAllowedKnown,          1, bool     );
        SENF_PARSER_BITFIELD ( stbcKnown,                      1, bool     );
        SENF_PARSER_SKIP_BITS(                                 7           );
        SENF_PARSER_BITFIELD ( partialAidKnown,                1, bool     );

        SENF_PARSER_SKIP_BITS(                                 2           );
        SENF_PARSER_BITFIELD ( beamformed,                     1, bool     );
        SENF_PARSER_BITFIELD ( ldpcExtraOfdmSymbol,            1, bool     ); 
        SENF_PARSER_BITFIELD ( shortGiNsymDisambiguation,      1, bool     );
        SENF_PARSER_BITFIELD ( guardInterval,                  1, bool     );
        SENF_PARSER_BITFIELD ( txOpPsNotAllowed,               1, bool     );
        SENF_PARSER_BITFIELD ( stbc,                           1, bool     );

        SENF_PARSER_FIELD    ( bandwidth,                      UInt8Parser );

        SENF_PARSER_BITFIELD ( mcs_user0,                      4, unsigned );
        SENF_PARSER_BITFIELD ( nss_user0,                      4, unsigned );
        SENF_PARSER_BITFIELD ( mcs_user1,                      4, unsigned );
        SENF_PARSER_BITFIELD ( nss_user1,                      4, unsigned );
        SENF_PARSER_BITFIELD ( mcs_user2,                      4, unsigned );
        SENF_PARSER_BITFIELD ( nss_user2,                      4, unsigned );
        SENF_PARSER_BITFIELD ( mcs_user3,                      4, unsigned );
        SENF_PARSER_BITFIELD ( nss_user3,                      4, unsigned );

        SENF_PARSER_SKIP_BITS(                                 4           );
        SENF_PARSER_BITFIELD ( coding_user3,                   1, unsigned );
        SENF_PARSER_BITFIELD ( coding_user2,                   1, unsigned );
        SENF_PARSER_BITFIELD ( coding_user1,                   1, unsigned );
        SENF_PARSER_BITFIELD ( coding_user0,                   1, unsigned );

        SENF_PARSER_FIELD    ( groupId,                        UInt8Parser );
        SENF_PARSER_FIELD    ( partialAid,                     UInt16LSBParser);
        
        SENF_PARSER_FINALIZE( RadiotapPacket_VHTParser );
    };

        struct RadiotapPacket_AMPDUStatusParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_FIELD ( referenceNumber,  UInt32Parser );
        SENF_PARSER_FIELD ( flags,            UInt16Parser );
        SENF_PARSER_FIELD ( crc,              UInt8Parser  );
        SENF_PARSER_FIELD ( unused,           UInt8Parser  );

        SENF_PARSER_FINALIZE( RadiotapPacket_AMPDUStatusParser );
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
    struct RadiotapPacket_HeaderParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        /*
         * mandatory fields
         */
        SENF_PARSER_FIELD( version, UInt8Parser     );
        //padding bits, currently unused, it simply aligns the fields onto natural word boundaries.
        SENF_PARSER_SKIP ( 1                        );
        SENF_PARSER_FIELD( length,  UInt16LSBParser );

        SENF_PARSER_PRIVATE_FIELD( presentFlags, UInt32LSBParser );

        SENF_PARSER_FINALIZE( RadiotapPacket_HeaderParser );

        SENF_PARSER_PRIVATE_FIELD( presentFlags_ext1, UInt32LSBParser );
        SENF_PARSER_PRIVATE_FIELD( presentFlags_ext2, UInt32LSBParser );

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
            MCS_INDEX               = 19,
            A_MPDU_STATUS_INDEX     = 20,
            VHT_INDEX               = 21,

            MAX_INDEX               = 21,

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
            MCS_FLAG                = (1<<MCS_INDEX),
            A_MPDU_STATUS_FLAG      = (1<<A_MPDU_STATUS_INDEX),
            VHT_FLAG                = (1<<VHT_INDEX),

            RADIOTOP_NS_FLAG        = (1<<RADIOTOP_NS_INDEX),
            VENDOR_NS_FLAG          = (1<<VENDOR_NS_INDEX),
            EXTENDED_BITMASK_FLAG   = (1<<EXTENDED_BITMASK_INDEX)
        };

        static unsigned const FIELD_SIZE[MAX_INDEX+2];
    };

    struct RadiotapPacket_FrameTypeParser : public PacketParserBase
    {
#       include SENF_FIXED_PARSER()

        SENF_PARSER_SKIP_BITS  (4                       );
        SENF_PARSER_BITFIELD_RO( frameType, 2, unsigned );
        SENF_PARSER_SKIP_BITS  (            2           );

        SENF_PARSER_FINALIZE( RadiotapPacket_FrameTypeParser );
    };

    struct RadiotapPacketParser : public RadiotapPacket_HeaderParser
    {
        RadiotapPacketParser(data_iterator i, state_type s);

        static const size_type init_bytes = RadiotapPacket_HeaderParser::fixed_bytes;

        size_type bytes() const;

        //-////////////////////////////////////////////////////////////////////////

#       define FIELD(name,type,index)                                                                \
            typedef type name ## _t;                                                                 \
            type name(unsigned extIndex = 0) { return parseField<type>(index, extIndex); }           \
            bool has_ ## name(unsigned extIndex = 0) { return currentTable()[(extIndex+1)*index]; }  \
            bool name ## Present(unsigned extIndex = 0) { return has_ ## name(extIndex); }           \
            type init_ ## name() { initField(index); return name(); }                                \
            void disable_ ## name() { disableField(index); }

        FIELD( tsft,              UInt64LSBParser,                      TSFT_INDEX              );

        // flags is special: disabling 'flags' must also disable the 'fcs' field
        typedef RadiotapPacket_FlagsParser flags_t;
        flags_t flags(unsigned extIndex = 0) { return parseField<flags_t>(FLAGS_INDEX, extIndex); }
        bool has_flags(unsigned extIndex = 0) { return currentTable()[(extIndex+1)*FLAGS_INDEX]; }
        bool flagsPresent(unsigned extIndex = 0) { return has_flags(extIndex); }
        flags_t init_flags() { initField(FLAGS_INDEX); return flags(); }
        void disable_flags() { disable_fcs(); disableField(FLAGS_INDEX); }

        FIELD( rate,              UInt8Parser,                          RATE_INDEX              );
        FIELD( channelOptions,    RadiotapPacket_ChannelOptionsParser,  CHANNEL_INDEX           );
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
        FIELD( rxFlags,           RadiotapPacket_RxFlagsParser,         RX_FLAGS_INDEX          );
        FIELD( txFlags,           RadiotapPacket_TxFlagsParser,         TX_FLAGS_INDEX          );
        FIELD( rtsRetries,        UInt8Parser,                          RTS_RETRIES_INDEX       );
        FIELD( dataRetries,       UInt8Parser,                          DATA_RETRIES_INDEX      );
        FIELD( mcs,               RadiotapPacket_MCSParser,             MCS_INDEX               );
        FIELD( vht,               RadiotapPacket_VHTParser,             VHT_INDEX               );
        FIELD( ampduStatus,       RadiotapPacket_AMPDUStatusParser,     A_MPDU_STATUS_INDEX     );

#       undef FIELD

        typedef UInt32Parser fcs_t;
        UInt32Parser fcs();
        bool has_fcs();
        UInt32Parser init_fcs();
        void disable_fcs();

        unsigned frameType();
        unsigned rateInKbps();
        unsigned bandwidth();

    private:
        static const size_type fixed_bytes = 0; // hide this member, just in case

        typedef boost::array<size_type, MAX_INDEX * 3 + 2> OffsetTable;
        typedef boost::unordered_map<std::size_t, OffsetTable> OffsetMap;

        //-////////////////////////////////////////////////////////////////////////
        // Offset table handling

        // Fills the offset table based on a packet
        static void parseOffsetTable(boost::uint8_t * data, int maxLength, OffsetTable & table);
        // Generate an offset table just from the present flags
        static void buildOffsetTable(boost::uint32_t presentFlags, OffsetTable & table);

        //-////////////////////////////////////////////////////////////////////////

        OffsetTable const & currentTable() const;
        OffsetTable const & getTable(boost::uint32_t presentFlags) const;

        template <class Parser>
        Parser parseField(unsigned index, unsigned extIndex);
        void initField(unsigned index);
        void disableField(unsigned index);

        size_type calculateSize() const;

        void updatePresentFlags(boost::uint32_t flags);
        void insertRemoveBytes(unsigned from, unsigned to, int bytes);

        static OffsetMap offsetMap_;
        OffsetTable const * currentTable_;

        friend struct RadiotapPacketType;
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
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( RadiotapPacket );
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
