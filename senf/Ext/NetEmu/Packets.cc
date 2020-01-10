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
    \brief Packets non-inline non-template implementation */

#include "Packets.hh"
//#include "Packets.ih"

// Custom includes
#include "Annotations.hh"
#include <senf/Utils/hexdump.hh>
#include <senf/Utils/Format.hh>
#include <senf/Packets/80211Bundle/RadiotapPacket.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_INSTANTIATE_TEMPLATE( senf::emu::PacketHeader                 );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::emu::WLANPacketHeader             );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::emu::EmulatedWirelessPacketHeader );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::emu::DVBPacketHeader              );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::emu::EthernetPacketHeader         );

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::PacketHeaderType

prefix_ void senf::emu::PacketHeaderType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias (os);
    os << "SENF Emulator packet header:\n"
       << senf::fieldName("ifaceType") << senf::format::dumpint(p->interfaceType().value()) << "\n"
       << senf::fieldName("sequenceNr") << p->sequenceNumber() << "\n"
       << senf::fieldName("timestamp") << " 0x"
       << std::hex << std::setw(16) << std::setfill('0') << std::right << p->timestamp() << " ("
       << std::dec << ClockService::abstime( ClockService::clock_type(p->timestamp())) << ")\n"
       << senf::fieldName("nodeId") << p->nodeId() << "\n";
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::WLANPacketHeaderType

namespace {
    // Arbitrary magic number used to identify the emulation header for WLAN packets
    // Defined to be the 32bit integer representation of the string 'WLAN' in network byte order
    boost::uint32_t const wlanMagic (0x574c414eu);

    SENF_PACKET_REGISTRY_REGISTER(
            senf::emu::InterfaceTypes, wlanMagic, senf::emu::WLANPacketHeader );
}

prefix_ senf::emu::WLANPacketHeaderType::factory_t
senf::emu::WLANPacketHeaderType::nextPacketType(packet p)
{
    if (p->mgt())
        return RadiotapPacket::factory();
    return EthernetPacket::factory();
}

prefix_ void senf::emu::WLANPacketHeaderType::finalize(packet p)
{
    if (p.next(senf::nothrow).is<RadiotapPacket>())
        p->mgt() = true;
}

prefix_ void senf::emu::WLANPacketHeaderType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias (os);
    os << "SENF Emulator WLAN packet header:\n";
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::EmulatedWirelessPacketHeaderType

prefix_ void senf::emu::EmulatedWirelessPacketHeaderType::dump(packet p,
                                                               std::ostream & os)
{
    boost::io::ios_all_saver ias (os);
    os << "SENF Emulator emulated wireless packet header:\n"
       << senf::fieldName("ifaceType") << format::dumpint(p->interfaceType().value()) << "\n"
       << senf::fieldName("txPower")   << p->txPower() << "\n"
       << senf::fieldName("modulationId") << p->modulationId() << "\n";
}

namespace {
    // Arbitrary magic number used to identify the emulation header for WLAN packets
    // Defined to be the 32bit integer representation of the string 'EmuW' in network byte order
    boost::uint32_t const emulatedWirelessMagic (0x456d7557);

    SENF_PACKET_REGISTRY_REGISTER(
            senf::emu::InterfaceTypes,
            emulatedWirelessMagic,
            senf::emu::EmulatedWirelessPacketHeader);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::DVBPacketHeaderType

namespace {
    // Arbitrary magic number used to identify the packet type.
    boost::uint32_t const dvbMagic (0x44564254u);

    SENF_PACKET_REGISTRY_REGISTER(
            senf::emu::InterfaceTypes, dvbMagic, senf::emu::DVBPacketHeader );
}

prefix_ void senf::emu::DVBPacketHeaderType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias (os);
    os << "SENF Emulator DVB packet header:\n";
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::EthernetPacketHeaderType

namespace {
    // Arbitrary magic number used to identify the packet type.
    boost::uint32_t const ethernetMagic (0x657468);

    SENF_PACKET_REGISTRY_REGISTER(
            senf::emu::InterfaceTypes, ethernetMagic, senf::emu::EthernetPacketHeader );
}

prefix_ void senf::emu::EthernetPacketHeaderType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias (os);
    os << "SENF Emulator Ethernet packet header:\n";
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ std::ostream & senf::emu::operator<<(std::ostream & os, dumpPacketInfo const & info)
{
    Packet p (info.packet.first());
    PacketHeader pi (p.find<PacketHeader>(senf::nothrow));

    os     << "Packet meta:\n";
    os     << "  size                    : " << p.size() << "\n";
    if (pi)
        os << "  recv time               : "
           << p.annotation<annotations::Timestamp>() << "\n";
    p.dump(os);

    DataPacket data (p.last().find<DataPacket>(senf::nothrow));
    if (data) senf::hexdump(data.data().begin(), data.data().end(), os, 16u);

    return os;
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
