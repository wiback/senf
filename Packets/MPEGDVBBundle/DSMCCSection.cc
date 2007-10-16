// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>
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
    \brief DSMCCSection non-inline non-template implementation */

#include "DSMCCSection.hh"
//#include "DSMCCSection.ih"

// Custom includes
#include <iomanip>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

//namespace {
//    senf::PacketRegistry<senf::EtherTypes>::RegistrationProxy<senf::EthVLanPacketType>
//        registerEthVLanPacket(0x8100);
//}


prefix_ void senf::DSMCCSectionType::dump(packet p, std::ostream & os)
{
    os << "DSMCC Section:\n"
       << std::hex
       << "  table_id: 0x" << unsigned(p->table_id()) << "\n"
       << "  section_syntax_indicator: " << p->ssi() << "\n"
       << "  private_indicator: " << p->private_indicator() << "\n"
       << "  reserved_1: " << p->reserved_1() << "\n"
       << std::dec
       << "  section_length: " << p->sec_length() << "\n"
       << std::hex 
       << "  table_id_extension: " << p->table_id_extension() << "\n"
       << "  reserved_2: " << p->reserved_2() << "\n"
       << "  version_number: " << p->version_num() << "\n"
       << "  current_next_indicator: " << p->curr_next_indicator() << "\n"
       << "  section_number: " << unsigned(p->sec_num()) << "\n"
       << "  last_section_number: " << unsigned(p->last_sec_num()) << "\n"
       << std::dec
       << "  CRC: " << unsigned(p->crc()) << "\n";
}

prefix_ senf::PacketParserBase::size_type senf::DSMCCSectionType::initSize()
{
    return parser::fixed_bytes + 32/8;
}

prefix_ senf::PacketParserBase::size_type senf::DSMCCSectionType::initHeadSize()
{
    return parser::fixed_bytes;
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
