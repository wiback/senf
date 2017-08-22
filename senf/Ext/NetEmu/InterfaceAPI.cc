// $Id:$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fokus.fraunhofer.de.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//       Stefan Bund <g0dil@berlios.de>

/** \file
    \brief InterfaceAPI non-inline non-template implementation */

#include "InterfaceAPI.hh"

// Custom includes
#include <time.h>
#include <unistd.h>
#include <boost/format.hpp>
#include <senf/Scheduler/ClockService.hh>
#include <senf/Utils/Console/SysInfo.hh>
#include <senf/Utils/Console/Sysdir.hh>
#include <senf/Utils/Statistics.hh>
#include <senf/Utils/String.hh>
#include "Version.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::Interface

unsigned senf::emu::Interface::nodeId_ (::getpid());

prefix_ void senf::emu::Interface::init()
{
    namespace kw = console::kw;
    namespace fty = console::factory;

    consoleDir().add("enable", fty::Command(&Interface::enable, this)
            .doc("Enable (ifconfig up) interface") );
    consoleDir().add("disable", fty::Command(&Interface::disable, this)
            .doc("Disable (ifconfig down) interface") );
    consoleDir().add("enabled", fty::Command(&Interface::enabled, this)
            .formatter(console::formatEnabledDisabled)
            .doc("Check, whether interface is enabled.") );
    consoleDir().add("id", fty::Command(SENF_MEMBINDFNP(void, Interface, id, (MACAddress const &)))
            .arg("id","new interface EUID")
            .overloadDoc("Set interface id (MACAddress)") );
    consoleDir().add("id", fty::Command(SENF_MEMBINDFNP(MACAddress, Interface, id, () const))
            .overloadDoc("Get interface id (MACAddress)") );
    consoleDir().add("deviceId", fty::Command(SENF_MEMBINDFNP(InterfaceDeviceId, Interface, deviceId, () const ))
            .overloadDoc("Get interface device id") );
    consoleDir().add("deviceId", fty::Command(SENF_MEMBINDFNP(InterfaceDeviceId, Interface, deviceId, () const ))
            .overloadDoc("Get interface device id") );
    consoleDir().add("device", fty::Command(SENF_MEMBINDFNP(std::string const &, Interface, device, () const ))
            .overloadDoc("Returns the interface device name") );
    consoleDir().add("type", fty::Command(&Interface::interfaceTypeName, this)
            .doc("Get interface type") );
    console::DirectoryNode & stats (consoleDir().add("stats", fty::Directory()));
    stats.add("statistics-interval", fty::Command<void (ClockService::clock_type)>(boost::ref(startStatistics))
            .arg("interval", "Statistics interval",
                    kw::parser=senf::parseClockServiceInterval)
            .doc("Set the interval, at which statistics output is generated from this interface.\n"
                    "Without setting an interval, NO statistics will be generated.") );

    index_ = 1;
    deviceId_ = InterfaceDeviceId::get(id());
    std::string name (senf::str(index_));
    while (interfaceDir().hasChild(name))
        name = senf::str(++index_);
    interfaceDir().add(name, consoleDir());
    console::DirectoryNode & iddir (console::provideDirectory(interfaceDir(),"by-id"));
    std::string idstr (senf::str(id()));
    if (! iddir.hasChild(idstr))
        iddir.add(idstr, fty::Link(consoleDir()));
}

prefix_ void senf::emu::Interface::id(MACAddress const & mac)
{
    MACAddress oldId = id_;
    v_id( mac);
    id_ = mac;
    console::DirectoryNode & iddir (console::provideDirectory(interfaceDir(), "by-id"));
    if (iddir.hasChild(senf::str(oldId)))
        iddir.remove(senf::str(oldId));
    iddir.add(senf::str(id()), console::factory::Link(consoleDir()));
    idChangedSignal( *this, oldId);
}

prefix_ boost::optional<senf::Statistics &> senf::emu::Interface::statistic(std::string const & name)
{
    Statistics::iterator i (statistics_.find(name));
    if (i != statistics_.end())
        return boost::optional<senf::Statistics &>( *(i->second));
    return boost::optional<senf::Statistics &>();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::Receiver

prefix_ void senf::emu::Receiver::init()
{
    namespace fty = console::factory;
    interface().consoleDir().add("promisc",
            fty::Command(SENF_MEMBINDFNP(void, Receiver, promisc, (bool) ))
        .overloadDoc("Enable/disable promiscuous mode.") );
    interface().consoleDir().add("promisc",
            fty::Command(SENF_MEMBINDFNP(bool, Receiver, promisc, () const))
        .overloadDoc("Get promiscuous state.") );
    interface().consoleDir().add("mcAdd",
            fty::Command(&Receiver::mcAdd, this)
        .overloadDoc("Add multicast address to interface.") );
    interface().consoleDir().add("mcDrop",
            fty::Command(&Receiver::mcDrop, this)
        .overloadDoc("Drop multicast address from interface.") );
    interface().consoleDir().add("flushRxQueues",
            fty::Command(&Receiver::flushRxQueues, this)
        .overloadDoc("Flushes the Rx queues where possible.") );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::Transmitter

prefix_ void senf::emu::Transmitter::init()
{
    namespace fty = console::factory;
    interface().consoleDir().add("mtu",
            fty::Command(SENF_MEMBINDFNP(void, Transmitter, mtu, (unsigned) ))
        .overloadDoc("Set MTU.") );
    interface().consoleDir().add("mtu",
            fty::Command(SENF_MEMBINDFNP(unsigned, Transmitter, mtu, () const))
        .overloadDoc("Get MTU.") );
    interface().consoleDir().add("flushTxQueues",
            fty::Command(&Transmitter::flushTxQueues, this)
        .overloadDoc("Flushes the Tx queues where possible.") );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    struct InterfaceDir
    {
        InterfaceDir() {
            namespace fty = senf::console::factory;
            dir.add("nodeId", fty::Command(SENF_FNP(unsigned, senf::emu::Interface::nodeId, ()))
                .doc("Get or set node id.\n"
                        "The nodeId defaults to the process id") );
            dir.add("nodeId", fty::Command(SENF_FNP(void, senf::emu::Interface::nodeId, (unsigned)))
                .arg("id","new node id") );
            senf::console::sysdir().add("interfaces", dir);
        }
        senf::console::ScopedDirectory<> dir;
    };

    senf::console::SysInfo::Proxy addSysInfo ("NetEmu",
            "SENF NetEmu: The PPI Network Emulator\n"
            "  (c) 2008-2015 Fraunhofer Institute for Open Communication Systems (FOKUS)\n"
            "  Contact: http://www.fokus.fraunhofer.de/en/net\n"
            "  Version: " SENF_NETEMU_LIB_VERSION "\n", 1);

    boost::uint16_t initIndex()
    {
        boost::uint32_t tm (::time(0));
        senf::ClockService::clock_type clk (senf::ClockService::now());
        tm = tm ^ (tm >> 16) ^ senf::ClockService::in_nanoseconds(clk) ^ ::getpid();
        return senf::emu::perturb(tm);
    }

    boost::uint16_t & macIdIndex()
    {
        static boost::uint16_t index (initIndex());
        return index;
    }

    // Just for the fun of it ... This is the AES/Rijndael substitution table
    // This is a bijective, non-linear transformation with no fixed points
    boost::uint8_t aesSubst[] = {
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b,
        0xfe, 0xd7, 0xab, 0x76, 0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
        0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, 0xb7, 0xfd, 0x93, 0x26,
        0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2,
        0xeb, 0x27, 0xb2, 0x75, 0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
        0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, 0x53, 0xd1, 0x00, 0xed,
        0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f,
        0x50, 0x3c, 0x9f, 0xa8, 0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
        0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, 0xcd, 0x0c, 0x13, 0xec,
        0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14,
        0xde, 0x5e, 0x0b, 0xdb, 0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
        0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, 0xe7, 0xc8, 0x37, 0x6d,
        0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f,
        0x4b, 0xbd, 0x8b, 0x8a, 0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
        0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, 0xe1, 0xf8, 0x98, 0x11,
        0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f,
        0xb0, 0x54, 0xbb, 0x16
    };

}

prefix_ senf::console::DirectoryNode & senf::emu::interfaceDir()
{
    static InterfaceDir interfaceDir;
    return interfaceDir.dir.node();
}

prefix_ boost::uint16_t senf::emu::perturb(boost::uint16_t value)
{
    boost::uint8_t high (aesSubst[(value>>8) & 0xff]);
    boost::uint8_t low (aesSubst[value & 0xffu]);
    return (((low+high) & 0xff)<<8) | ((low-high)&0xff);
}

prefix_ void senf::emu::macIdSeed(boost::uint16_t seed)
{
    macIdIndex() = seed;
}

prefix_ senf::MACAddress senf::emu::uniqueMAC(boost::uint32_t magic)
{
    // These MAC's are NOT random, they are UNIQUE ...
    boost::uint16_t index (perturb(macIdIndex()++));
    MACAddress eui;
    eui[0] = ((magic>>24) & 0xfcu) | 0x02u;
    eui[1] = (magic>>16) & 0xffu;
    eui[2] = (magic>>8) & 0xffu;
//    eui[3] = magic & 0xffu;
    eui[3] = ::getpid() & 0xffu; // increase the randomness space by 8 more bits
    eui[4] = (index>>8) & 0xffu;
    eui[5] = index & 0xffu;
    return eui;
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
