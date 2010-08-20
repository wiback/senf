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

// Definition of RadiotapPacket non-inline non-template functions

#include "RadiotapPacket.hh"
//#include "RadiotapPacket.ih"

// Custom includes
#include "WLANPacket.hh"
#include <boost/io/ios_state.hpp>

extern "C" {
#   include "radiotap/radiotap_iter.h"
}

#define prefix_
///////////////////////////////cc.p//////////////////////////////////////

prefix_ void senf::RadiotapPacketParser::fillOffsetTable(boost::uint8_t * data, int maxLength,
                                                         OffsetTable & table)
{
    struct ieee80211_radiotap_iterator iter;
    ieee80211_radiotap_iterator_init(&iter,
                                     (struct ieee80211_radiotap_header *)data,
                                     maxLength,
                                     0);
    while (ieee80211_radiotap_iterator_next(&iter)==0) {
        if (iter.is_radiotap_ns &&
            iter.this_arg_index <= int(senf::RadiotapPacketParser::MAX_INDEX)) {
            table[iter.this_arg_index] = iter.this_arg - data;
            std::cerr << ">> " << iter.this_arg_index << " " << table[iter.this_arg_index] << "\n";
        }
    }
    table[MAX_INDEX+1] = iter.this_arg - data + iter.this_arg_size;
    std::cerr << ">> size " << table[MAX_INDEX+1] << "\n";
}

prefix_ senf::RadiotapPacketParser::OffsetTable const &
senf::RadiotapPacketParser::offsetTable(boost::uint32_t presentFlags)
{
    typedef std::map<boost::uint32_t, OffsetTable> OffsetMap;
    static OffsetMap offsetMap;

    OffsetMap::iterator i (offsetMap.find(presentFlags));
    if (i == offsetMap.end()) {
        OffsetTable table;
        fillOffsetTable(&(*data().begin()), data().size(), table);
        i = offsetMap.insert(std::make_pair(presentFlags, table)).first;
    }
    return i->second;
}


#define DUMP_OPTIONAL_FIELD(name, sign, desc)                           \
    if (p->name ## Present())                                           \
        os << senf::fieldName(desc) << sign(p->name())                  \
           << std::endl;

prefix_ void senf::RadiotapPacketType::dump(packet p, std::ostream &os)
{
    boost::io::ios_all_saver ias(os);
    os << "Radiotap:\n"
       << senf::fieldName("version") << unsigned(p->version()) << "\n"
       << senf::fieldName("length")  << unsigned(p->length()) << "\n";
    // TODO: flags, channelOptions
    DUMP_OPTIONAL_FIELD( tsft,              unsigned, "MAC timestamp"        );
    DUMP_OPTIONAL_FIELD( rate,              unsigned, "rate"                 );
    DUMP_OPTIONAL_FIELD( fhss,              unsigned, "FHSS"                 );
    DUMP_OPTIONAL_FIELD( dbmAntennaSignal,  signed,   "antenna signal (dBm)" );
    DUMP_OPTIONAL_FIELD( dbmAntennaNoise,   signed,   "antenna noise (dBm)"  );
    DUMP_OPTIONAL_FIELD( lockQuality,       unsigned, "lock quality"         );
    DUMP_OPTIONAL_FIELD( txAttenuation,     unsigned, "tx attenuation"       );
    DUMP_OPTIONAL_FIELD( dbTxAttenuation,   unsigned, "tx attenuation (dB)"  );
    DUMP_OPTIONAL_FIELD( dbmTxAttenuation,  signed,   "tx attenuation (dBm)" );
    DUMP_OPTIONAL_FIELD( antenna,           unsigned, "antenna"              );
    DUMP_OPTIONAL_FIELD( dbAntennaSignal,   unsigned, "antenna signal (dB)"  );
    DUMP_OPTIONAL_FIELD( dbAntennaNoise,    unsigned, "antenna noise (dB)"   );
    DUMP_OPTIONAL_FIELD( headerFcs,         unsigned, "FCS (in header)"      );
    if (p->flagsPresent() && p->flags().fcsAtEnd())
        os << senf::fieldName("FCS (at end)") << unsigned(p->fcs()) << "\n";
}

#undef DUMP_OPTIONAL_FIELD

prefix_ void senf::RadiotapPacketType::finalize(packet p)
{
    p->length() << p->calculateSize();
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
senf::RadiotapPacketType::nextPacketRange(packet p)
{
    size_type h (senf::bytes(p.parser()));
    size_type t (p->flagsPresent() && p->flags().fcsAtEnd() ? 4 : 0);
    return p.size() < h+t
        ? no_range()
        : optional_range( range(p.data().begin() + h, p.data().end() - t) );
}

///////////////////////////////cc.e////////////////////////////////////////
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
