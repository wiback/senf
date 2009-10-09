// $Id: WLANPacket.cc 1449 2009-09-25 23:03:48Z g0dil $
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
#include "WLANBeaconPacket.hh"

#include <senf/Packets/Packets.hh>
#include <boost/io/ios_state.hpp>

#define prefix_

namespace {
}

prefix_ void senf::WLANBeaconPacketType::dump(packet p, std::ostream &os)
{
    boost::io::ios_all_saver ias(os);    
    os << "WLAN Beacon:\n"
       << senf::fieldName("timestamp") << unsigned( p->timestamp()) << "\n";
}

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
