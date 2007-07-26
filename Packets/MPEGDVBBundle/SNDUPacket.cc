// $Id: DSMCCSection.cc 350 2007-07-25 08:26:41Z g0dil $
//
// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

#include "SNDUPacket.hh"
//#include "SNDUPacket.ih"

// Custom includes
#include <iomanip>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

//namespace {
//    senf::PacketRegistry<senf::EtherTypes>::RegistrationProxy<senf::EthVLanPacketType>
//        registerEthVLanPacket(0x8100);
//}


prefix_ senf::PacketParserBase::size_type senf::Parse_SNDUPacket::bytes()
    const
{
    if ( d_bit() )
        return 2 + 2 + 4;  // D-Bit + 15 bits length + 16 bits type field + 32 bits crc
    else
        return 2 + 2 + 4 + 6;  // + 6 Byte NPA destination address
}

prefix_ void senf::SNDUPacketType::dump(packet p, std::ostream & os)
{
    os << "SNDUPacket:\n";
}

prefix_ senf::PacketParserBase::size_type senf::SNDUPacketType::initSize()
{
    return 2 + 2 + 4;  // D-Bit + 15 bits length + 16 bits type field + 32 bits crc
}

prefix_ senf::PacketParserBase::size_type senf::SNDUPacketType::initHeadSize()
{
    return 2 + 2;  // D-Bit + 15 bits length + 16 bits type field
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
