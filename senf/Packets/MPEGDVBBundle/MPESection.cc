//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief MPESection non-inline non-template implementation */

#include "MPESection.hh"
//#include "MPESection.ih"

// Custom includes
#include <iomanip>
#include <boost/io/ios_state.hpp>
#include <senf/Packets/DefaultBundle/LlcSnapPacket.hh>
#include <senf/Packets/DefaultBundle/IPv4Packet.hh>
#include <senf/Packets/DefaultBundle/IPv6Packet.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_INSTANTIATE_TEMPLATE( senf::MPESection );

prefix_ boost::uint32_t senf::MPESectionParser::calcCrc()
    const
{
    return std::for_each(
            data().begin(),
            boost::prior(data().end(), 4),
            crc32_t() ).checksum();
}

prefix_ void senf::MPESectionType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "MPE Section:\n"
       << std::hex
       << senf::fieldName("table_id")                  << "0x" << unsigned(p->table_id()) << "\n"
       << senf::fieldName("section syntax indicator")  << p->section_syntax_indicator() << "\n"
       << senf::fieldName("private indicator")         << p->private_indicator() << "\n"
       << std::dec
       << senf::fieldName("section length")            << p->section_length() << "\n"
       << std::hex
       << senf::fieldName("MAC address 6")             << "0x" << unsigned(p->mac_addr_6()) << "\n"
       << senf::fieldName("MAC address 5")             << "0x" << unsigned(p->mac_addr_5()) << "\n"
       << senf::fieldName("payload scrambling ctrl")   << "0x" << p->payload_scrmbl_ctrl() << "\n"
       << senf::fieldName("address scrambling ctrl")   << "0x" << p-> addr_scrmbl_ctrl()  << "\n"
       << senf::fieldName("LLC/SNAP flag")             << "0x" << p->llc_snap_flag() << "\n"
       << senf::fieldName("current next indicator")    << "0x" << p->curr_next_indicator() << "\n"
       << senf::fieldName("section number")            << "0x" << unsigned(p->section_num()) << "\n"
       << senf::fieldName("last section number")       << "0x" << unsigned(p->last_section_num()) << "\n"
       << senf::fieldName("real time parameters")      << "\n"
       << senf::fieldName("  delta_t")                 << "0x" << unsigned(p->real_time_parameters().delta_t()) << "\n"
       << senf::fieldName("  table boundary")          << "0x" << unsigned(p->real_time_parameters().table_boundary()) << "\n"
       << senf::fieldName("  frame boundary")          << "0x" << unsigned(p->real_time_parameters().frame_boundary()) << "\n"
       << senf::fieldName("  address")                 << "0x" << unsigned(p->real_time_parameters().address()) << "\n"
       << std::dec
       << senf::fieldName("crc")                       << unsigned(p->crc()) << "\n";
}

prefix_ senf::PacketParserBase::size_type senf::MPESectionType::initSize()
{
    return parser::fixed_bytes + 32/8;
}

prefix_ senf::PacketParserBase::size_type senf::MPESectionType::initHeadSize()
{
    return parser::fixed_bytes;
}

prefix_ senf::PacketInterpreterBase::factory_t senf::MPESectionType::nextPacketType(packet p)
{
    if (p.data().size() > initSize()+1) {
        if (p->llc_snap_flag())
            return LlcSnapPacket::factory();
        if (p->ip_datagram_version().value() == 4)
            return IPv4Packet::factory();
        if (p->ip_datagram_version().value() == 6)
            return IPv6Packet::factory();
    }
    return no_factory();
}

prefix_ void senf::MPESectionType::finalize(packet p)
{
    p->llc_snap_flag() = p.next(nothrow) && p.next().is<LlcSnapPacket>() ? 1 : 0;
    p->section_length() = p.data().size() - 3;
    p->crc() = p->calcCrc();
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
