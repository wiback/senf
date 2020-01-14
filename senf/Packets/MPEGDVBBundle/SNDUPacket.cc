//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
    PacketInterpreterBase::factory_t f (no_factory());
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
