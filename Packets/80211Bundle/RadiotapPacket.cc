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
#include "../../Packets/Packets.hh"
#include <boost/io/ios_state.hpp>

#define prefix_


prefix_ void senf::RadiotapPacketType::dump(packet p, std::ostream &os)
{
    boost::io::ios_all_saver ias(os);
    os << "Radiotap:\n"
       << "  Version             : " << unsigned( p->version()) << "\n"
       << "  Length              : " << unsigned( p->length()) << "\n";
    if (p->has_tsft())
        os  << "  MAC timestamp       : " << unsigned( p->tsft()) << "\n";
    // TODO: flags
    if (p->has_rate())
        os  << "  Rate                : " << unsigned( p->rate()) << "\n";
    // TODO: channelOptions
    if (p->has_fhss())
        os  << "  FHSS                : " << unsigned( p->fhss()) << "\n";
    if (p->has_dbmAntennaSignal())
        os  << "  Antenna Signal (dBm): " << signed( p->dbmAntennaSignal()) << "\n";
    if (p->has_dbmAntennaNoise())
        os  << "  Antenna Noise (dBm) : " << signed( p->dbmAntennaNoise()) << "\n";
    if (p->has_lockQuality())
        os  << "  Lock Quality        : " << unsigned( p->lockQuality()) << "\n";
    if (p->has_txAttenuation())
        os  << "  txAttenuation       : " << unsigned( p->txAttenuation()) << "\n";
    if (p->has_dbTxAttenuation())
        os  << "  dbTxAttenuation     : " << unsigned( p->dbTxAttenuation()) << "\n";
    if (p->has_dbmTxAttenuation())
        os  << "  dbmTxAttenuation    : " << signed( p->dbmTxAttenuation()) << "\n";
    if (p->has_antenna())
        os  << "  Antenna             : " << unsigned( p->antenna()) << "\n";
    if (p->has_dbAntennaSignal())
        os  << "  Antenna Signal (dB) : " << unsigned( p->dbAntennaSignal()) << "\n";
    if (p->has_dbAntennaNoise())
        os  << "  Antenna Noise (dB)  : " << unsigned( p->dbAntennaNoise()) << "\n";
    if (p->has_fcs())
        os  << "  FCS                 : " << unsigned( p->fcs()) << "\n";
}

prefix_ void senf::RadiotapPacketType::finalize(packet p)
{
    p->length() << senf::bytes( p.parser());
}

prefix_ senf::PacketInterpreterBase::factory_t senf::RadiotapPacketType::nextPacketType(packet p)
{
    return WLANPacket::factory();
}


#undef prefix_
