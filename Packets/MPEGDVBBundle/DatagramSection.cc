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
    \brief DatagramSection non-inline non-template implementation */

#include "DatagramSection.hh"
//#include "DatagramSection.ih"

// Custom includes
#include <iomanip>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

//namespace {
//    senf::PacketRegistry<senf::EtherTypes>::RegistrationProxy<senf::EthVLanPacketType>
//        registerEthVLanPacket(0x8100);
//}


prefix_ void senf::DatagramSectionType::dump(packet p, std::ostream & os)
{
    os << "Datagram Section (MPE):\n"
       << std::hex
       << "  table_id: 0x" << unsigned(p->table_id()) << "\n"
       << "  section_syntax_indicator: " << p->ssi() << "\n"
       << "  private_indicator: " << p->private_indicator() << "\n"
       << std::dec
       << "  section_length: " << p->sec_length() << "\n"
       << std::hex 
       << "  MAC_address_6: " << unsigned(p->mac_addr_6()) << "\n"
       << "  MAC_address_5: " << unsigned(p->mac_addr_5()) << "\n"
       << "  payload_scrambling_control: " << p->payload_scrmbl_ctrl() << "\n"
       << "  address_scrambling_control: " << p-> addr_scrmbl_ctrl()  << "\n"
       << "  LLC_SNAP_flag: " << p->llc_snap_flag() << "\n"
       << "  current_next_indicator: " << p->curr_next_indicator() << "\n"
       << "  section_number: " << unsigned(p->sec_num()) << "\n"
       << "  last_section_number: " << unsigned(p->last_sec_num()) << "\n"
       << "  MAC_address_4: " << unsigned(p->mac_addr_4()) << "\n"
       << "  MAC_address_3: " << unsigned(p->mac_addr_3()) << "\n"
       << "  MAC_address_2: " << unsigned(p->mac_addr_2()) << "\n"
       << "  MAC_address_1: " << unsigned(p->mac_addr_1()) << "\n"
       << std::dec
       << "  CRC: " << unsigned(p->crc()) << "\n";
}

prefix_ senf::PacketParserBase::size_type senf::DatagramSectionType::initSize()
{
    return parser::fixed_bytes + 32/8;
}

prefix_ senf::PacketParserBase::size_type senf::DatagramSectionType::initHeadSize()
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
