// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <g0dil@berlios.de>
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
    \brief Packet non-inline non-template implementation */

//#include "Packet.ih"

// Custom includes
#include "Packets.hh"

//#include "Packet.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::Packet senf::Packet::checkNext()
    const
{
    PacketInterpreterBase::optional_range r (ptr()->nextPacketRange());
    if (r && ! r->empty()) {
        factory_t factory (ptr()->nextPacketType());
        if (factory)
            return parseNextAs(factory);
        else
            return parseNextAs<DataPacket>();
    }
    return Packet();
}

prefix_ senf::Packet senf::Packet::checkLast()
    const
{
    Packet p (*this);
    Packet n (p.next());
    while (n) {
        p = n;
        n = p.next();
    }
    return p;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "Packet.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
