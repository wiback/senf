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
    \brief TAPEthernetInterface non-inline non-template implementation */

#include <linux/sockios.h>
#include <linux/filter.h>
#include "TAPEthernetInterface.hh"

// Custom includes

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::TAPEthernetInterfaceNet

prefix_ senf::emu::detail::TAPEthernetInterfaceNet::TAPEthernetInterfaceNet(std::string const & device)
    : socket (TapSocketHandle(device)),
      dev_(device), ctrl_(device),
      annotatorRx_(true, false, ctrl_.hardwareAddress()), annotatorTx_(false, false),
      netOutput (annotatorRx_.output), netInput (annotatorTx_.input)
{
    socket.blocking(false);

    senf::ppi::connect(source.output, annotatorRx_.input);
    senf::ppi::connect(annotatorTx_.output, sink.input);
}

prefix_ void senf::emu::detail::TAPEthernetInterfaceNet::assignSockets(bool on)
{
    if (on) {
        source.handle(socket);
        sink.handle(socket);
    } else {
        source.handle( TapSocketHandle(senf::noinit));
        sink.handle( TapSocketHandle(senf::noinit));
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::TAPEthernetInterface

namespace {
    struct DisableInterfaceGuard
    {
        DisableInterfaceGuard(senf::emu::Interface & iface)
            : iface_ (iface), enabled_ (iface_.enabled())
            { if (enabled_) iface_.disable(); }

        ~DisableInterfaceGuard()
            { if (enabled_) iface_.enable(); }

        senf::emu::Interface & iface_;
        bool enabled_;
    };
}

prefix_ senf::emu::TAPEthernetInterface::TAPEthernetInterface(std::string const & device)
    : TAPEthernetInterfaceNet(device),
      EthernetInterface (netOutput, netInput),
      pvid_(VLanId::None), accessMode_(false)
{
    BidirectionalWiredInterface::init();
    HardwareInterface::init();

    namespace fty = console::factory;

    consoleDir()
        .add("mmapStats", fty::Command(&TAPEthernetInterface::dumpMmapStats, this)
        .doc("dumps and resets the socket's RX/TX statistics"));
    consoleDir()
        .add("maxBurst", fty::Command(
                 SENF_MEMBINDFNP(void, TAPEthernetInterface, maxBurst, (unsigned)))
             .doc("set max burst rate"));
    consoleDir()
        .add("maxBurst", fty::Command(
                 SENF_MEMBINDFNP(unsigned, TAPEthernetInterface, maxBurst, () const))
             .doc("get max burst rate"));
    consoleDir()
        .add("pvid", fty::Command(
                 SENF_MEMBINDFNP(bool, TAPEthernetInterface, pvid, (VLanId const &, bool)))
             .doc( "enables filtering for a specific PVID (VLAN ID must be 0...4095)"));
    consoleDir()
        .add("pvid", fty::Command(
                 SENF_MEMBINDFNP(VLanId const &, TAPEthernetInterface, pvid, () const))
             .doc( "report the currently configured PVID (-1 means none)"));


    console::provideDirectory(interfaceDir(),"by-device").add(dev_, fty::Link(consoleDir()));

    annotatorRx_.id(id());

    if (enabled()) {
        assignSockets(true);
    }
}

prefix_ senf::emu::TAPEthernetInterface::~TAPEthernetInterface()
{
    try {
        console::provideDirectory(interfaceDir(),"by-device").remove(device());
    } catch (...) {};
}

prefix_ std::string const & senf::emu::TAPEthernetInterface::v_device()
    const
{
    return dev_;
}

prefix_ void senf::emu::TAPEthernetInterface::v_enable()
{
    if (! enabled()) {
        ctrl_.up();
    }

    TAPEthernetInterfaceNet::assignSockets(true);
}

prefix_ void senf::emu::TAPEthernetInterface::v_disable()
{
    if (enabled()) {
        ctrl_.down();
    }

    TAPEthernetInterfaceNet::assignSockets(false);
}

prefix_ bool senf::emu::TAPEthernetInterface::v_enabled()
    const
{
    return ctrl_.isUp();
}

prefix_ void senf::emu::TAPEthernetInterface::v_id(MACAddress const & mac)
{
    DisableInterfaceGuard guard (*this);
    ctrl_.hardwareAddress(mac);
    annotatorRx_.id(mac);
}

prefix_ senf::MACAddress senf::emu::TAPEthernetInterface::v_id()
    const
{
    return ctrl_.hardwareAddress();
}

prefix_ bool senf::emu::TAPEthernetInterface::v_promisc()
    const
{
    return ctrl_.promisc();
}

prefix_ void senf::emu::TAPEthernetInterface::v_promisc(bool p)
{
    ctrl_.promisc(p);
}

prefix_ bool senf::emu::TAPEthernetInterface::v_annotationMode()
    const
{
    return annotatorRx_.annotate();
}

prefix_ void senf::emu::TAPEthernetInterface::v_annotationMode(bool a)
{
    annotatorRx_.annotate(a);
}

prefix_ unsigned senf::emu::TAPEthernetInterface::v_mtu()
    const
{
    return ctrl_.mtu();
}

prefix_ void senf::emu::TAPEthernetInterface::v_mtu(unsigned v)
{
    ctrl_.mtu(v);
}

prefix_ void senf::emu::TAPEthernetInterface::v_mcAdd(MACAddress const & address)
{
    throw senf::SystemException("multicast not supported on TAP Interfaces", ENOSYS);
}

prefix_ void senf::emu::TAPEthernetInterface::v_mcDrop(MACAddress const & address)
{
    throw senf::SystemException("multicast not supported on TAP Interfaces", ENOSYS);
}


prefix_ void senf::emu::TAPEthernetInterface::v_flushRxQueues()
{
    TAPEthernetInterfaceNet::source.flush();
}

prefix_ bool senf::emu::TAPEthernetInterface::pvid(VLanId const & p, bool accessMode)
{
    if (!accessMode and p.stag())
        return false;
    
    pvid_ = p;
    accessMode_ = accessMode;

    if (pvid_) {
        if (accessMode_) {
            annotatorRx_.insertTag(pvid_);
            annotatorTx_.removeTag(pvid_);
        } else {
            annotatorRx_.removeTag(pvid_);
            annotatorTx_.insertTag(pvid_);
        }
    } else {
        annotatorRx_.clearTag();
        annotatorTx_.clearTag();
    }

    return true;
}

prefix_ std::uint32_t senf::emu::TAPEthernetInterface::vlanMismatchRx()
{
    return TAPEthernetInterfaceNet::annotatorRx_.vlanMismatch();
}

prefix_ std::uint32_t senf::emu::TAPEthernetInterface::vlanMismatchTx()
{
    return TAPEthernetInterfaceNet::annotatorTx_.vlanMismatch();
}

prefix_ unsigned senf::emu::TAPEthernetInterface::maxBurst()
    const
{
    return source.maxBurst();
}

prefix_ void senf::emu::TAPEthernetInterface::maxBurst(unsigned maxBurst)
{
    source.maxBurst(maxBurst);
}

prefix_ void senf::emu::TAPEthernetInterface::dumpMmapStats(std::ostream & os)
{
    if (TAPEthernetInterfaceNet::socket.valid()) {
        os << "Socket acvtive (fd=" << socket.protocol().fd() << ", available " << socket.protocol().available() << "), RxStats: ";
        source.rxStats().dump(os); os << " TxStats: "; sink.txStats().dump(os); os << std::endl;
    } else {
        os << "Socket closed. Not stats available." << std::endl;
    }
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
