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
    os 	    << "Radiotap:\n"
            << "  Version             : " << unsigned (p->version()) << "\n"
            << "  Length              : " << unsigned (p->length()) << "\n";
    if (p->has_dbmAntennaSignal())
        os  << "  Signal              : " << signed (p-> dbmAntennaSignal()) << "\n";
    if (p->has_tsft())
        os  << "  MAC timestamp       : " << unsigned (p->tsft()) << "\n";
    if (p->has_dbmAntennaNoise())
        os  << "  Noise               : " << signed (p-> dbmAntennaNoise()) << "\n";



}

prefix_ void senf::RadiotapPacketType::finalize(packet p)
{
    //TODO
    p->length() << p.size();
}

prefix_ senf::PacketInterpreterBase::factory_t senf::RadiotapPacketType::nextPacketType(packet p)
{
    return WLANPacket::factory();
}


#undef prefix_
