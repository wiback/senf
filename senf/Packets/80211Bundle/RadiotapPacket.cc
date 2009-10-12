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

#define prefix_
///////////////////////////////cc.p//////////////////////////////////////

namespace {
    #define DUMP_OPTIONAL_FIELD(name, sign, desc)           \
        if (p->has_##name())                                \
            os << senf::fieldName(desc) << sign( p->name()) \
               << std::endl;
}

prefix_ void senf::RadiotapPacketType::dump(packet p, std::ostream &os)
{
    boost::io::ios_all_saver ias(os);
    os << "Radiotap:\n"
       << senf::fieldName("version") << unsigned( p->version()) << "\n"
       << senf::fieldName("length")  << unsigned( p->length()) << "\n";
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
    if (p->has_headerFcs())
        os << senf::fieldName("FCS")     << unsigned( p->fcs()) << "\n";
}

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
