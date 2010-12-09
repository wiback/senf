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
    \brief DSMCCSection non-inline non-template implementation */

#include "SNDUPacket.hh"
//#include "SNDUPacket.ih"

// Custom includes
#include <iomanip>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ boost::uint32_t senf::SNDUPacketParser::calcCrc()
    const
{
    return std::for_each(
            data().begin(),
            boost::prior(data().end(), 4),
            ule_crc32() ).checksum();
}

//prefix_ senf::SNDUPacketType::key_t senf::SNDUPacketType::nextPacketKey(packet p)
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
    senf::PacketInterpreterBase::factory_t f (no_factory());
    if (p->type() < 1536) {
        PacketRegistry<senf::ULEExtHeaderTypes>::Entry const * e (
            PacketRegistry<senf::ULEExtHeaderTypes>::lookup( p->type(), nothrow ));
        if (e) f = e->factory();
    }
    else {
        PacketRegistry<senf::EtherTypes>::Entry const * e (
            PacketRegistry<senf::ULEExtHeaderTypes>::lookup( p->type(), nothrow ));
        if (e) f = e->factory();
    }
    return f;
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
       << senf::fieldName("d_bit")  << p->d_bit() << "\n"
       << senf::fieldName("length") << unsigned(p->length()) << "\n"
       << std::hex
       << senf::fieldName("type")   << "0x" << unsigned(p->type()) << "\n"
       << std::dec
       << senf::fieldName("crc")    << unsigned(p->crc()) << "\n";
}

prefix_ senf::PacketParserBase::size_type senf::SNDUPacketType::initSize()
{
    return 2 + 2 + 4;  // D-Bit + 15 bits length + 16 bits type field + 32 bits crc
}

prefix_ senf::PacketParserBase::size_type senf::SNDUPacketType::initHeadSize()
{
    return 2 + 2;  // D-Bit + 15 bits length + 16 bits type field
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
