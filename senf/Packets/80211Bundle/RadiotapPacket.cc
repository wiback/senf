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


#include "RadiotapPacket.hh"
//#include "RadiotapPacket.ih"

// Custom includes
#include <memory.h>
#include <endian.h>
#include <boost/io/ios_state.hpp>
#include "WLANPacket.hh"
#include "MCSInfo.hh"

// Compatibility with glibc < 2.9
#if !defined(htole32) && !defined(le32toh)
#  include <byteswap.h>
#  include <arpa/inet.h>
#  define htole32(x)  (bswap_32(htonl(x)))
#  define le32toh(x)  (ntohl(bswap_32(x)))
#endif

extern "C" {
#   include "radiotap/radiotap_iter.h"
}

#define MHZ_TO_KHZ(freq) ((freq) * 1000)
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))


#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_INSTANTIATE_TEMPLATE( senf::RadiotapPacket );

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// Offset table management

senf::RadiotapPacketParser::OffsetMap senf::RadiotapPacketParser::offsetMap_;

prefix_ void senf::RadiotapPacketParser::parseOffsetTable(boost::uint8_t * data, int maxLength,
        OffsetTable & table)
{
    ieee80211_radiotap_iterator iter;
    ieee80211_radiotap_iterator_init(
            &iter, reinterpret_cast<ieee80211_radiotap_header *>(data), maxLength, 0);
    unsigned size (8u);
    unsigned extIndex = 0;
    int last_index = 255;
    while (ieee80211_radiotap_iterator_next(&iter) == 0) {
        if (not iter.is_radiotap_ns)
            continue;
        if (iter.this_arg_index > int(RadiotapPacketParser::MAX_INDEX))
            continue;
        if (iter.this_arg_index < last_index)
            extIndex++;
        last_index = iter.this_arg_index;
        table[extIndex * iter.this_arg_index] = iter.this_arg - data;
        // We need to set size here in the loop since the iter fields are only valid
        // when at least one present bit is set ...
        size = iter.this_arg - data + iter.this_arg_size;
    }
    table[MAX_INDEX+1] = size;
}

prefix_ void senf::RadiotapPacketParser::buildOffsetTable(boost::uint32_t presentFlags, OffsetTable & table)
{
    SENF_ASSERT(!(presentFlags & ( (1<<IEEE80211_RADIOTAP_RADIOTAP_NAMESPACE) |
                                   (1<<IEEE80211_RADIOTAP_VENDOR_NAMESPACE) |
                                   (1<<IEEE80211_RADIOTAP_EXT) )),
                "Extended or vendor fields not supported");

    ieee80211_radiotap_header header;
    memset(&header, 0, sizeof(header));
    // header.it_version = 0;

    // Iterating this packet will generate invalid addresses but we don't care since neither
    // radiotap.c nor we will ever dereference those pointers, we just calculate the offsets.
    // This works, as long as we don't support extension headers ...
    header.it_len = 0xFFFF;
    // Note that all data in the header is little endian!
    header.it_present = htole32(presentFlags);

    parseOffsetTable((boost::uint8_t*)&header, header.it_len, table);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::RadiotapPacketParser

unsigned const senf::RadiotapPacket_HeaderParser::FIELD_SIZE[] = {
    8, 1, 1, 4, 2, 1, 1, 2, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 0, 3, 8, 12 };

prefix_ senf::UInt32Parser senf::RadiotapPacketParser::init_fcs()
{
    if (!has_fcs()) {
        protect(), data().insert(data().end(), 4u, 0u);
        init_flags().fcsAtEnd_() = true;
    }
    return fcs();
}

prefix_ void senf::RadiotapPacketParser::disable_fcs()
{
    if (has_fcs()) {
        validate(RadiotapPacket_HeaderParser::fixed_bytes+4);
        data().erase(data().end()-4, data().end());
        flags().fcsAtEnd_() = false;
    }
}

prefix_ senf::RadiotapPacketParser::OffsetTable const &
senf::RadiotapPacketParser::getTable(boost::uint32_t presentFlags)
    const
{
    OffsetTable & table (offsetMap_[boost::hash_value(presentFlags)]);
    if (SENF_UNLIKELY(! table[MAX_INDEX+1]))
        buildOffsetTable(presentFlags, table);
    return table;
}

prefix_ void senf::RadiotapPacketParser::insertRemoveBytes(unsigned from, unsigned to, int bytes)
{
    data_iterator b (i() + from);
    data_iterator e (i() + to);
    if (bytes >= 0) {
        // Insert some bytes cleaning the old bytes to 0 first
        std::fill(b, e,  0u);
        if (bytes > 0)
            // need to protect the parser since data().insert() invalidates iterators
            protect(), data().insert(e, bytes, 0u);
    }
    else { // bytes < 0
        // Remove some bytes ...
        // remember: bytes is negative ...
        if (b < e + bytes)
            std::fill(b, e + bytes, 0u);
        data().erase(e + bytes, e);
    }
}

prefix_ void senf::RadiotapPacketParser::updatePresentFlags(boost::uint32_t flags)
{
    if (flags == presentFlags())
        return;
    validate(bytes());

    OffsetTable const & oldTable (currentTable());
    OffsetTable const & newTable (getTable(flags));
    unsigned b (RadiotapPacket_HeaderParser::fixed_bytes);
    int cumulativeNewBytes (0);

    for (unsigned index (0); index <= MAX_INDEX; ++index) {
        // Skip any unchanged fields
        for (; index <= MAX_INDEX+1
                 && ((oldTable[index] == 0 && newTable[index] == 0)
                     || (oldTable[index]+cumulativeNewBytes == newTable[index])); ++index)
            if (newTable[index] != 0)
                b = newTable[index] + FIELD_SIZE[index];
        if (index > MAX_INDEX+1)
            break;
        // Now skip over all changed fields
        for (; ! (oldTable[index]!=0 && newTable[index]!=0); ++index) ;
        // (The condition is not needed here since the last table entry MAX_INDEX+1 is
        // always != 0 in both tables, BUT newer g++ complains about array access above the bounds :-/
        if (index > MAX_INDEX+1)
            break;
        // index now either points to
        // a) an entry set in both tables
        // b) at the end of the table which contains the total length
        // (remember: the table has a size of MAX_INDEX+2 entries !!)
        // in both cases, the difference between the new and old size
        // is found from the difference between the old and the new table
        // entry
        int newBytes (newTable[index] - oldTable[index] - cumulativeNewBytes);
        insertRemoveBytes(b, oldTable[index] + cumulativeNewBytes, newBytes);
        cumulativeNewBytes += newBytes;
        b = newTable[index] + FIELD_SIZE[index];
    }
    length() += cumulativeNewBytes;
    presentFlags() = flags;
    currentTable_ = &newTable;
}

prefix_ unsigned senf::RadiotapPacketParser::rateInKbps()
{
    if (SENF_LIKELY(mcsPresent()))
        return WLAN_MCSInfo::getRate(
                mcs().mcsIndex(),
                mcs().bandwidth() == 1 ? 40 : 20,
                mcs().guardInterval());
    if (ratePresent())
        return rate() * 1000 / 2;
    return 0;
}

prefix_ unsigned senf::RadiotapPacketParser::bandwidth()
{
    unsigned vhtBandwidths[] = {
            MHZ_TO_KHZ(20),  MHZ_TO_KHZ(40),  MHZ_TO_KHZ(40),  MHZ_TO_KHZ(40),
            MHZ_TO_KHZ(80),  MHZ_TO_KHZ(80),  MHZ_TO_KHZ(80),  MHZ_TO_KHZ(80),
            MHZ_TO_KHZ(80),  MHZ_TO_KHZ(80),  MHZ_TO_KHZ(80),  MHZ_TO_KHZ(160),
            MHZ_TO_KHZ(160), MHZ_TO_KHZ(160), MHZ_TO_KHZ(160), MHZ_TO_KHZ(160),
            MHZ_TO_KHZ(160), MHZ_TO_KHZ(160), MHZ_TO_KHZ(160), MHZ_TO_KHZ(160),
            MHZ_TO_KHZ(160), MHZ_TO_KHZ(160), MHZ_TO_KHZ(160), MHZ_TO_KHZ(160),
            MHZ_TO_KHZ(160), MHZ_TO_KHZ(160) };

    if (mcsPresent())
        return mcs().bandwidth() == 1 ? MHZ_TO_KHZ(40) : MHZ_TO_KHZ(20);
    if (vhtPresent() and vht().bandwidth() < ARRAY_SIZE(vhtBandwidths))
        return vhtBandwidths[vht().bandwidth()];
    return 0;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::RadiotapPacketType

prefix_ void senf::RadiotapPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "Radiotap:\n"
       << senf::fieldName("version") << unsigned(p->version()) << '\n'
       << senf::fieldName("length")  << unsigned(p->length()) << '\n';

#   define FIELD(name, sign, desc)                                      \
        if (p->name ## Present())                                       \
            os << senf::fieldName(desc) << sign(p->name()) << '\n';

#   define ENTER(name)                                                  \
        if (p->name ## Present()) {                                     \
            packet::Parser::name ## _t subparser (p->name());

#   define SUBFIELD(name, sign, desc)                                   \
        os << senf::fieldName(desc) << sign(subparser.name()) << '\n';

#   define LEAVE()                                                      \
        }

#   define START_FLAGS(desc)                                            \
        os << senf::fieldName(desc);

#   define FLAG(name, desc)                                             \
        if (subparser.name()) os << desc " "

#   define END_FLAGS()                                                  \
        os << '\n';

static const char * MCSbandwidthDesc[] = { "20", "40", "20L", "20U" };
static const char * MCSguardIntervalDesc[] = { "long", "short" };
static const char * MCShtFormatDesc[] = { "mixed", "greenfield" };
static const char * MCSfecTypeDesc[] = { "BCC", "LDPC" };
static const char * MCSmcsIndexDesc[] = { };

#   define MCS_FLAG(name, desc, longDesc)                                   \
        if (subparser.name ## Known()) {                                    \
            os << senf::fieldName("  " desc) << unsigned(subparser.name()); \
            if (longDesc)                                                   \
                os << " (" << MCS ## name ## Desc[subparser.name()] << ")"; \
            os << "\n"; }


static const char * VHTpartialAidDesc[] = { };
static const char * VHTgroupIdDesc[] = { };
static const char * VHTbandwidthDesc[] = { };
static const char * VHTbeamformedDesc[] = { "yes", "no" };
static const char * VHTldpcExtraOfdmSymbolDesc[] = { "yes", "no" };
static const char * VHTshortGiNsymDisambiguationDesc[] = { "yes", "no" };
static const char * VHTguardIntervalDesc[] = { "long", "short" };
static const char * VHTtxOpPsNotAllowedDesc[] = { "yes", "no" };
static const char * VHTstbcDesc[] = { "yes", "no" };


#   define VHT_FLAG(name, desc, longDesc)                               \
        if (subparser.name ## Known()) {                                    \
            os << senf::fieldName("  " desc) << unsigned(subparser.name()); \
            if (longDesc)                                                   \
                os << " (" << VHT ## name ## Desc[subparser.name()] << ")"; \
            os << "\n"; }



    FIELD           ( tsft,              boost::uint64_t, "MAC timestamp"        );
    ENTER           ( flags                                                      );
      START_FLAGS   (                                     "flags"                );
        FLAG        (     shortGI,                            "ShortGI"          );
        FLAG        (     badFCS,                             "BadFCS"           );
        FLAG        (     fcsAtEnd,                           "FCSatEnd"         );
        FLAG        (     fragmentation,                      "Frag"             );
        FLAG        (     wep,                                "WEP"              );
        FLAG        (     shortPreamble,                      "ShortPreamble"    );
        FLAG        (     cfp,                                "CFP"              );
      END_FLAGS     (                                                            );
    LEAVE           (                                                            );
    FIELD           ( rate,              unsigned,        "rate"                 );
    ENTER           ( channelOptions                                             );
      SUBFIELD      (     freq,          unsigned,        "channel frequency"    );
      START_FLAGS   (                                     "channel flags"        );
        FLAG        (     flag2ghz,                           "2GHz"             );
        FLAG        (     ofdm,                               "OFDM"             );
        FLAG        (     cck,                                "CCK"              );
        FLAG        (     turbo,                              "Turbo"            );
        FLAG        (     quarterRateChannel,                 "Rate/4"           );
        FLAG        (     halfRateChannel,                    "Rate/2"           );
        FLAG        (     gsm,                                "GSM"              );
        FLAG        (     staticTurbo,                        "StaticTurbo"      );
        FLAG        (     gfsk,                               "GFSK"             );
        FLAG        (     cckOfdm,                            "CCK+OFDM"         );
        FLAG        (     passive,                            "Passive"          );
        FLAG        (     flag5ghz,                           "5GHz"             );
      END_FLAGS     (                                                            );
    LEAVE           (                                                            );
    FIELD           ( fhss,              unsigned,        "FHSS"                 );
    FIELD           ( dbmAntennaSignal,  signed,          "antenna signal (dBm)" );
    FIELD           ( dbmAntennaNoise,   signed,          "antenna noise (dBm)"  );
    FIELD           ( lockQuality,       unsigned,        "lock quality"         );
    FIELD           ( txAttenuation,     unsigned,        "tx attenuation"       );
    FIELD           ( dbTxAttenuation,   unsigned,        "tx attenuation (dB)"  );
    FIELD           ( dbmTxAttenuation,  signed,          "tx attenuation (dBm)" );
    FIELD           ( antenna,           unsigned,        "antenna"              );
    FIELD           ( dbAntennaSignal,   unsigned,        "antenna signal (dB)"  );
    FIELD           ( dbAntennaNoise,    unsigned,        "antenna noise (dB)"   );
    ENTER           ( rxFlags                                                    );
      START_FLAGS   (                                     "rx flags"             );
        FLAG        (     badPlcp,                            "BadPLCP"          );
      END_FLAGS     (                                                            );
    LEAVE           (                                                            );
    ENTER           ( txFlags                                                    );
      START_FLAGS   (                                     "tx flags"             );
        FLAG        (     fail,                               "Fail"             );
        FLAG        (     txRts,                              "RTS"              );
        FLAG        (     txCts,                              "CTS"              );
      END_FLAGS     (                                                            );
    LEAVE           (                                                            );
    FIELD           ( rtsRetries,        unsigned,        "rts retries"          );
    FIELD           ( dataRetries,       unsigned,        "data retries"         );
    ENTER           ( mcs                                                        );
      START_FLAGS   (                                    "known MCS information" );
        FLAG        (     bandwidthKnown,                     "bandwidth"        );
        FLAG        (     mcsIndexKnown,                      "MCS index"        );
        FLAG        (     guardIntervalKnown,                 "guard interval"   );
        FLAG        (     htFormatKnown,                      "HT format"        );
        FLAG        (     fecTypeKnown,                       "FEC type"         );
      END_FLAGS     (                                                            );
      os <<                                            "  MCS information\n"      ;
        MCS_FLAG    (     bandwidth,                          "bandwidth",      1);
        MCS_FLAG    (     guardInterval,                      "guard interval", 1);
        MCS_FLAG    (     htFormat,                           "HT format",      1);
        MCS_FLAG    (     fecType,                            "FEC type",       1);
        MCS_FLAG    (     mcsIndex,                           "MCS index",      0);
    LEAVE           (                                                            );
    ENTER           ( vht                                                        );
      START_FLAGS   (                                    "known VHT information" );
        FLAG        (     partialAidKnown,                    "Partial AID"      );
        FLAG        (     groupIdKnown,                       "Group ID"         );
        FLAG        (     bandwidthKnown,                     "Bandwidth"        );
        FLAG        (     beamformedKnown,                    "Beamformed"       );
        FLAG        (     ldpcExtraOfdmSymbolKnown,           "LDPC Extra OFDM Symbol");
        FLAG        (     shortGiNsymDisambiguationKnown,     "Short GI NSYM Disambiguation");
        FLAG        (     guardIntervalKnown,                 "Guard Interval"   );
        FLAG        (     txOpPsNotAllowedKnown,              "TxOp PS Not Allowed");
        FLAG        (     stbcKnown,                          "STBC"             );
      END_FLAGS     (                                                            );
      os <<                                            "  VHT information\n"      ;
        VHT_FLAG    (     partialAid,                         "Partial AID"   , 0);
        VHT_FLAG    (     groupId,                            "GroupId"       , 0);
        VHT_FLAG    (     bandwidth,                          "Bandwidth Index", 0);
        VHT_FLAG    (     beamformed,                         "Beamformed",    1);
        VHT_FLAG    (     ldpcExtraOfdmSymbol,                "LDPC Extra OFDM Symbol", 1);
        VHT_FLAG    (     shortGiNsymDisambiguation,          "Short GI NSYM Disambiguation", 1);
        VHT_FLAG    (     guardInterval,                      "Guard Interval", 1);
        VHT_FLAG    (     txOpPsNotAllowed,                   "TxOp PS Not Allowed", 1);
        VHT_FLAG    (     stbc,                               "STBC",           1);
    LEAVE           (                                                            );
    
    if (p->flagsPresent() && p->flags().fcsAtEnd())
        os << senf::fieldName("fcs") << unsigned(p->fcs()) << '\n';

#   undef END_FLAGS
#   undef FLAG
#   undef START_FLAGS
#   undef LEAVE
#   undef SUBFIELD
#   undef ENTER
#   undef FIELD
#   undef MCS_FLAG
#   undef VHT_FLAG
}

prefix_ void senf::RadiotapPacketType::init(packet p)
{
    // ?? Why the heck do we need the +0? Otherwise we get an
    // 'undefined reference to 'RadiotapPacket_HeaderParser::fixed_bytes'
    p->length() << RadiotapPacket_HeaderParser::fixed_bytes+0;
}

prefix_ senf::PacketInterpreterBase::factory_t senf::RadiotapPacketType::nextPacketType(packet p)
{
    static factory_t frameTypeFactory[] = { WLANPacket_MgtFrame::factory(),
                                            WLANPacket_CtrlFrame::factory(),
                                            WLANPacket_DataFrame::factory(),
                                            no_factory() };
    return frameTypeFactory[p->frameType()];
}

prefix_ senf::RadiotapPacketType::optional_range
senf::RadiotapPacketType::nextPacketRange(packet const & p)
{
    parser rtParser (p.parser());
    size_type h (senf::bytes(rtParser));
    size_type t (rtParser.flagsPresent() && rtParser.flags().fcsAtEnd() ? 4 : 0);
    return p.size() < h+t
        ? no_range()
        : optional_range( range(p.data().begin() + h, p.data().end() - t) );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
