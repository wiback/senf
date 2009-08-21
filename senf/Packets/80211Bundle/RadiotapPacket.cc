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

// Definition of non-inline non-template functions

// Custom includes
#include "RadiotapPacket.hh"
#include "WLANPacket.hh"
#include <senf/Packets/Packets.hh>
#include <boost/io/ios_state.hpp>

#define prefix_


prefix_ void senf::RadiotapPacketType::dump(packet p, std::ostream &os)
{
    boost::io::ios_all_saver ias(os);
    os << "Radiotap:\n"
       <<     "  version                 : " << unsigned( p->version()) << "\n"
       <<     "  length                  : " << unsigned( p->length()) << "\n";
    if (p->has_tsft())
        os << "  MAC timestamp           : " << unsigned( p->tsft()) << "\n";
    // TODO: flags
    if (p->has_rate())
        os << "  rate                    : " << unsigned( p->rate()) << "\n";
    // TODO: channelOptions
    if (p->has_fhss())
        os << "  FHSS                    : " << unsigned( p->fhss()) << "\n";
    if (p->has_dbmAntennaSignal())
        os << "  antenna signal (dBm)    : " << signed( p->dbmAntennaSignal()) << "\n";
    if (p->has_dbmAntennaNoise())
        os << "  antenna noise (dBm)     : " << signed( p->dbmAntennaNoise()) << "\n";
    if (p->has_lockQuality())
        os << "  lock quality            : " << unsigned( p->lockQuality()) << "\n";
    if (p->has_txAttenuation())
        os << "  tx attenuation          : " << unsigned( p->txAttenuation()) << "\n";
    if (p->has_dbTxAttenuation())
        os << "  tx attenuation (dB)     : " << unsigned( p->dbTxAttenuation()) << "\n";
    if (p->has_dbmTxAttenuation())
        os << "  tx attenuation (dBm)    : " << signed( p->dbmTxAttenuation()) << "\n";
    if (p->has_antenna())
        os << "  antenna                 : " << unsigned( p->antenna()) << "\n";
    if (p->has_dbAntennaSignal())
        os << "  antenna signal (dB)     : " << unsigned( p->dbAntennaSignal()) << "\n";
    if (p->has_dbAntennaNoise())
        os << "  antenna noise (dB)      : " << unsigned( p->dbAntennaNoise()) << "\n";
    if (p->has_headerFcs())
        os << "  FCS                     : " << unsigned( p->fcs()) << "\n";
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

#undef prefix_
