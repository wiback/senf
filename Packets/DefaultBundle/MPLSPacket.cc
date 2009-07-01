// $Id$
//
// Copyright (C) 2006
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

/** \file
    \brief MPLSPacket non-inline non-template implementation */

#include "MPLSPacket.hh"
//#include "MPLSPacket.ih"

// Custom includes
#include <iomanip>
#include <boost/io/ios_state.hpp>

#include "EthernetPacket.hh"

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    senf::PacketRegistry<senf::EtherTypes>::RegistrationProxy<senf::MPLSPacket>
        registerMPLSPacket (0x8848);
}


// senf::MPLSPacketType

prefix_ void senf::MPLSPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "MPLS Header: \n"
       << "  label : " << p->label() << "\n"
       << "  traffic class : " << p->tc() << "\n"
       << "  bottom of stack : " << p->s() << "\n"
       << "  Time-to-live : " << p->ttl() << "\n";
}

prefix_ senf::PacketInterpreterBase::factory_t senf::MPLSPacketType::nextPacketType(packet p)
{
    if (p->s()) //last MPLS header
        return no_factory();
    else
        return MPLSPacket::factory();
}

prefix_ void senf::MPLSPacketType::finalize(packet p)
{
    if (p.prev(senf::nothrow).is<MPLSPacket>()) //not the last label on the MPLS stack
        p->s_() << false;
    else
        p->s_() << true;
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
