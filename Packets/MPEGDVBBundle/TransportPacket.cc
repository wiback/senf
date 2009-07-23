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
    \brief TransportPacket non-inline non-template implementation */

#include "TransportPacket.hh"
//#include "TransportPacket.ih"

// Custom includes
#include <iomanip>
#include <boost/io/ios_state.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ void senf::TransportPacketParser::init_fields()
    const
{
    sync_byte() = TransportPacketType::SYNC_BYTE;
    transport_error_indicator() = 0;
    transport_scrmbl_ctrl() = 0;
    adaptation_field_ctrl() = 0x1u;
}

prefix_ senf::UInt8Parser senf::TransportPacketParser::pointer_field()
    const
{
    return pointer_field_().get<1>(); 
}

prefix_ void senf::TransportPacketParser::setPUSI(bool pusi)
    const
{
    if (pusi) pointer_field_().init<1>();
    else      pointer_field_().init<0>();
}
            
prefix_ void senf::TransportPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "TransportPacket:\n"
       << std::hex
       <<     "  syncByte                : 0x" << unsigned(p->sync_byte()) << "\n"
       <<     "  transport error ind.    : 0x" << unsigned(p->transport_error_indicator()) << "\n"
       <<     "  payload unit start ind. : 0x" << unsigned(p->pusi()) << "\n"
       <<     "  transport priority      : 0x" << unsigned(p->transport_priority()) << "\n"
       << std::dec
       <<     "  pid                     : " << unsigned(p->pid()) << "\n"
       << std::hex
       <<     "  transport scrambling c. : 0x" << unsigned(p->transport_scrmbl_ctrl()) << "\n"
       <<     "  adaptation field ctrl   : 0x" << unsigned(p->adaptation_field_ctrl()) << "\n"
       <<     "  continuity counter      : 0x" << unsigned(p->continuity_counter()) << "\n";
    if (p->pusi())
        os << "  pointer field           : 0x" << unsigned(p->pointer_field()) << "\n";
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
