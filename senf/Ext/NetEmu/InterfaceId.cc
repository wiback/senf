// $Id:$
//
// Copyright (C) 2012
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
//       Thorsten Horstmann <Thorsten.Horstmann@fokus.fraunhofer.de>

/** \file
    \brief InterfaceId non-inline non-template implementation */

#include "InterfaceId.hh"

// Custom includes
#include <limits>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>
#include <senf/Socket/Protocols/Raw/MACAddress.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    template <typename T>
    T readTokenFromFile(boost::filesystem::path const & path)
    {
        boost::filesystem::ifstream file;
        file.exceptions( std::ifstream::failbit | std::ifstream::badbit);
        try {
            file.open(path, std::ios::in);
            T token;
            file >> token;
            file.close();
            return token;
        } catch (std::exception &) {
            return T();
        }
    }

    int readHexFromFile(boost::filesystem::path const & path)
    {
        try {
            return std::stoi(readTokenFromFile<std::string>(path), 0, 16);
        } catch (std::exception &) {
            return 0;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::InterfaceDeviceId

prefix_ senf::emu::InterfaceDeviceId const senf::emu::InterfaceDeviceId::None (InterfaceDeviceId(0,0,0));

prefix_ senf::emu::InterfaceDeviceId::InterfaceDeviceId()
    : bus_(Unknown), vendor_(0), device_(0)
{}

prefix_ senf::emu::InterfaceDeviceId::InterfaceDeviceId(Bus_t b, boost::uint16_t v, boost::uint16_t d)
    : bus_(b), vendor_(v), device_(d)
{}

prefix_ senf::emu::InterfaceDeviceId::InterfaceDeviceId(boost::uint8_t b, boost::uint16_t v, boost::uint16_t d)
    : bus_(Unknown), vendor_(v), device_(d)
{
    if (b == PCI)  bus_ = PCI;
    if (b == USB)  bus_ = USB;
    if (b == PCIe) bus_ = PCIe;
}

prefix_ senf::emu::InterfaceDeviceId senf::emu::InterfaceDeviceId::get(senf::MACAddress const & addr)
{
    boost::filesystem::directory_iterator end_itr;
    for (boost::filesystem::directory_iterator itr ("/sys/class/net"); itr != end_itr; ++itr) {
        if (not boost::filesystem::is_symlink(itr->path()))
            continue;
        if (readTokenFromFile<senf::MACAddress>(itr->path()/"address") != addr)
            continue;
        if (not boost::filesystem::exists(itr->path()/"device"))
            continue;

        boost::uint16_t vendor (readHexFromFile(itr->path()/"device"/"vendor"));
        boost::uint16_t device (readHexFromFile(itr->path()/"device"/"device"));
        std::string b ("Unknown");
        if (boost::filesystem::exists(itr->path()/"device"/"subsystem"))
            b = boost::filesystem::canonical(itr->path()/"device"/"subsystem").filename().string();
        Bus_t bus (Unknown);
        if (b == "pci")
            bus = PCI;
        if (b == "pci_express")
            bus = PCIe;
        if (b == "usb")
            bus = USB;

        return InterfaceDeviceId(bus, vendor, device);
    }
    return InterfaceDeviceId();
}

std::ostream & senf::emu::operator<<(std::ostream & os, InterfaceDeviceId const & id)
{
    senf::console::format(id, os);
    return os;
}

prefix_ bool senf::emu::InterfaceDeviceId::operator<(InterfaceDeviceId const & other)
    const
{
    if (bus_ != other.bus_)
        return bus_ < other.bus_;
    if (vendor_ != other.vendor_)
        return vendor_ < other.vendor_;
    return device_ < other.device_;
}

prefix_ bool senf::emu::InterfaceDeviceId::operator==(InterfaceDeviceId const & other)
    const
{
    return bus_ == other.bus_    and
        vendor_ == other.vendor_ and
        device_ == other.device_;
}

prefix_ senf::emu::InterfaceDeviceId::Bus_t senf::emu::InterfaceDeviceId::bus()
    const
{
    return bus_;
}

prefix_ boost::uint16_t senf::emu::InterfaceDeviceId::vendor()
    const
{
    return vendor_;
}

prefix_ boost::uint16_t senf::emu::InterfaceDeviceId::device()
    const
{
    return device_;
}

///////////////////////////////////////////////////////////////////////////
// senf::emu::InterfaceIdFactoryBase

prefix_ senf::MACAddress senf::emu::InterfaceIdFactoryBase::getId()
{
    return v_getId();
}

prefix_ void senf::emu::InterfaceIdFactoryBase::releaseId(MACAddress const & id)
{
    v_releaseId(id);
}

///////////////////////////////////////////////////////////////////////////
// senf::emu::SimpleInterfaceIdFactory

prefix_ senf::MACAddress senf::emu::SimpleInterfaceIdFactory::v_getId()
{
    boost::uint16_t id (calcFirstFreeId());
    if (id != 0) {
        idSet_.insert( id);
        MACAddress mac (id);
        mac[0] |= 0x02;
        return mac;
    }
    return MACAddress::None;
}

prefix_ void senf::emu::SimpleInterfaceIdFactory::v_releaseId(MACAddress const & id)
{
     idSet_.erase( boost::uint16_t(id[4]) << 16 | boost::uint16_t(id[5]));
}

prefix_ boost::uint32_t senf::emu::SimpleInterfaceIdFactory::calcFirstFreeId()
{
    if (idSet_.empty())
        return 1;
    if (idSet_.size() == std::numeric_limits<uint16_t>::max())
        return 0;
    if (*idSet_.begin() > 1)
        return 1;
    IdSet::const_iterator i = std::adjacent_find(
            idSet_.begin(), idSet_.end(), boost::lambda::_1+1 < boost::lambda::_2 );
    if (i == idSet_.end()) --i;
    return *i + 1;
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
