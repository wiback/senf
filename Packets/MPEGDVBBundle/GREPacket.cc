// $Id:DatagramSection.cc 327 2007-07-20 10:03:44Z tho $
//
// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Joachim Kaeber <joachim.kaeber@fokus.fraunhofer.de>
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
    \brief GREPacket non-inline non-template implementation */

#include "GREPacket.hh"
//#include "GREPacket.ih"

// Custom includes
#include <iomanip>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ void senf::GREPacketType::dump(packet p, std::ostream & os)
{
    /*
    os << "GREPacket:\n"
       << std::hex
       << "  syncByte: 0x" << unsigned(p->sync_byte()) << "\n"
       << "  transport_error_indicator: 0x" << unsigned(p->transport_error_indicator()) << "\n"
       << "  payload_unit_start_indicator (pusi): 0x" << unsigned(p->pusi()) << "\n"
       << "  transport_priority: 0x" << unsigned(p->transport_priority()) << "\n"
       << std::dec
       << "  pid: " << unsigned(p->pid()) << "\n"
       << std::hex
       << "  transport_scrambling_control: 0x" << unsigned(p->transport_scrmbl_ctrl()) << "\n"
       << "  adaptation_field_control: 0x" << unsigned(p->adaptation_field_ctrl()) << "\n"
       << "  continuity_counter: 0x" << unsigned(p->continuity_counter()) << "\n";
    */
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
