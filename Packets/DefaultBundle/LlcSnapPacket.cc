// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief LlcSnapPacket non-inline non-template implementation */

#include "LlcSnapPacket.hh"
//#include "LlcSnapPacket.ih"

// Custom includes
#include <iomanip>
#include <boost/io/ios_state.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ void senf::LlcSnapPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "LLC/SNAP\n"
       << std::hex << std::setfill('0')
       << "  LLC\n"
       << "    DSAP: 0x" << unsigned(p->dsap()) << "\n"
       << "    SSAP: 0x" << unsigned(p->ssap()) << "\n"
       << "    controlId: 0x" << unsigned(p->ctrl()) << "\n"
       << "  SNAP\n"       
       << "    ProtocolId: 0x" << std::setw(6) << unsigned(p->protocolId()) << "\n"
       << "    type_length: 0x" << std::setw(4) << unsigned(p->type_length()) << "\n";
}

prefix_ senf::PacketInterpreterBase::factory_t senf::LlcSnapPacketType::nextPacketType(packet p)
{
    if (p->type_length() >= 1536) {
        PkReg_Entry const * e;
        e = PacketRegistry<senf::EtherTypes>::lookup( p->type_length(), nothrow );
        return e ? e->factory() : no_factory();
    }
    if (p->type_length() <= 1500)
        return EthernetPacket::factory();
    return no_factory();
}

prefix_ void senf::LlcSnapPacketType::finalize(packet p)
{
    optional_key_t k (key(p.next(nothrow)));
    if (k)
        p->type_length() << k;
    else if (p.next().is<EthernetPacket>())
        p->type_length() << p.next().data().size();
    else
        ///\fixme Is this correct ?? we at least need an 'if (! p.next().is<DataPacket>() )'
        p->type_length() << 0;
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
