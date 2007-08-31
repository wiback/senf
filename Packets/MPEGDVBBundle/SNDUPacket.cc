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

#include "SNDUPacket.hh"
//#include "SNDUPacket.ih"

// Custom includes
#include <iomanip>
#include "../../Utils/hexdump.hh"
#include "../../Packets/DefaultBundle/EthernetPacket.hh"


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

prefix_ boost::uint32_t senf::Parse_SNDUPacket::calcCrc()
    const
{
    return std::for_each(
            data().begin(), 
            boost::prior(data().end(), 4), 
            ule_crc32() ).checksum();
}

//prefix_ senf::SNDUPacketType::registry_key_t senf::SNDUPacketType::nextPacketKey(packet p)
//{
//    return p->type(); 
//}

prefix_ void senf::SNDUPacketType::init(packet p)
{
    p->init();
}

prefix_ senf::PacketInterpreterBase::factory_t senf::SNDUPacketType::nextPacketType(packet p)
{
    if (p.data().size() < 8)
        return no_factory();
    PkReg_Entry const * e;
    if (p->type() < 1536)
        e = PacketRegistry<senf::ULEExtHeaderTypes>::lookup( p->type(), nothrow );
    else
        e = PacketRegistry<senf::EtherTypes>::lookup( p->type(), nothrow );
    return e ? e->factory() : no_factory();
}

prefix_ senf::PacketInterpreterBase::optional_range 
senf::SNDUPacketType::nextPacketRange(packet p) 
{
    if (p.data().size() < 8)
        return no_range();
    
    size_type sz = 2 + 2;  // D-Bit + 15 bits length + 16 bits type field
    if (! p->d_bit() )
        sz += 6;  // + 6 Byte NPA destination address
    return range(
            boost::next(p.data().begin(), sz),
            boost::prior(p.data().end(), 4));  // - 32 bits crc
}

prefix_ void senf::SNDUPacketType::dump(packet p, std::ostream & os)
{
    os << "SNDUPacket:\n"
       << std::dec
       << "  d_bit: " << p->d_bit() << "\n"
       << "  length: " << unsigned(p->length()) << "\n"
       << std::hex
       << "  type: 0x" << unsigned(p->type()) << "\n"
       << std::dec
       << "  crc: " << unsigned(p->crc()) << "\n";
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
