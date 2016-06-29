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
//       Christian Niephaus <cni@berlios.de>

/** \file
    \brief HardwareEthernetInterface non-inline non-template implementation */

#include <linux/sockios.h>
#include <linux/filter.h>
#include "HardwareEthernetInterface.hh"

// Custom includes

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::HardwareEthernetInterfaceNet

prefix_ senf::emu::detail::HardwareEthernetInterfaceNet::HardwareEthernetInterfaceNet()
    : socket (senf::noinit), source (socket), sink (socket),
      netOutput (annotator_.output), netInput (sink.input)
{
    senf::ppi::connect(source.output, annotator_.input);
}

prefix_ void senf::emu::detail::HardwareEthernetInterfaceNet::assignSockets(ConnectedMMapPacketSocketHandle & socket_)
{
    socket = socket_;
    source.handle(socket);
    sink.handle(socket);
}

prefix_ unsigned senf::emu::detail::HardwareEthernetInterfaceNet::rcvBuf()
{
    if (socket)
        return socket.protocol().rcvbuf();
    return 0;
}

prefix_ void senf::emu::detail::HardwareEthernetInterfaceNet::rcvBuf(unsigned rcvbuf)
{
    if (socket)
        socket.protocol().rcvbuf(rcvbuf);
}

prefix_ unsigned senf::emu::detail::HardwareEthernetInterfaceNet::sndBuf()
{
    if (socket)
        return socket.protocol().sndbuf();
    return 0;
}

prefix_ void senf::emu::detail::HardwareEthernetInterfaceNet::sndBuf(unsigned sndbuf)
{
    if (socket)
        socket.protocol().sndbuf(sndbuf);
}

prefix_ void senf::emu::detail::HardwareEthernetInterfaceNet::setupBPF(senf::MACAddress const & hwAddr, bool srcOnly)
{
    if (!socket)
        return;

    // BPF: tcpdump -i eth1 -dd 'not ether src <self>
    static struct sock_filter BPF_code_src[] = {
        { 0x20, 0, 0, 0x00000008 },
        { 0x15, 0, 3, (boost::uint32_t(hwAddr[3]) << 24) | (boost::uint32_t(hwAddr[2]) << 16) | (boost::uint32_t(hwAddr[1]) << 8) | boost::uint32_t(hwAddr[0]) },
        { 0x28, 0, 0, 0x00000006 },
        { 0x15, 0, 1, (boost::uint32_t(hwAddr[5]) << 8) | boost::uint32_t(hwAddr[4])},
        { 0x06, 0, 0, 0x00000000 },
        { 0x06, 0, 0, 0x0000ffff }
    };

    // BPF: tcpdump -i eth1 -dd 'not ether src <self> and not ether dst <self>'
    static struct sock_filter BPF_code_src_dst[] = {
        { 0x20, 0, 0, 0x00000008 },
        { 0x15, 0, 3, (boost::uint32_t(hwAddr[3]) << 24) | (boost::uint32_t(hwAddr[2]) << 16) | (boost::uint32_t(hwAddr[1]) << 8) | boost::uint32_t(hwAddr[0]) },
        { 0x28, 0, 0, 0x00000006 },
        { 0x15, 0, 1, (boost::uint32_t(hwAddr[5]) << 8) | boost::uint32_t(hwAddr[4])},
        { 0x20, 0, 0, 0x00000002 },
        { 0x15, 0, 3, (boost::uint32_t(hwAddr[3]) << 24) | (boost::uint32_t(hwAddr[2]) << 16) | (boost::uint32_t(hwAddr[1]) << 8) | boost::uint32_t(hwAddr[0]) },
        { 0x28, 0, 0, 0x00000000 },
        { 0x15, 0, 1, (boost::uint32_t(hwAddr[5]) << 8) | boost::uint32_t(hwAddr[4])},
        { 0x06, 0, 0, 0x00000000 },
        { 0x06, 0, 0, 0x0000ffff }
    };

    if (srcOnly)
        socket.protocol().attachSocketFilter(BPF_code_src);
    else
        socket.protocol().attachSocketFilter(BPF_code_src_dst);
}

prefix_ void senf::emu::detail::HardwareEthernetInterfaceNet::clearBPF()
{
    if (!socket)
        return;

    socket.protocol().detachSocketFilter();
}



//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::HardwareEthernetInterface

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

prefix_ senf::emu::HardwareEthernetInterface::HardwareEthernetInterface(std::string const & device)
    : EthernetInterface (netOutput, netInput), dev_ (device), ctrl_ (dev_),
      rcvBufSize_ (4096), sndBufSize_ (96*1024), qlen_ (512), pvid_(std::uint16_t(-1))
{
    EthernetInterface::init();
    HardwareInterface::init();

    namespace fty = console::factory;

    consoleDir()
        .add("device",
             fty::Command(SENF_MEMBINDFNP(std::string, HardwareEthernetInterface, device, () const))
             .overloadDoc("Get Ethernet network device name.") );
    consoleDir()
        .add("mmapStats", fty::Command(&HardwareEthernetInterface::dumpMmapStats, this)
        .doc("dumps and resets the MMAP RX/TX statistics"));
    consoleDir()
        .add("queue", qAlgorithm().consoleDir());
    consoleDir()
        .add("sndBuf", fty::Command(
                 SENF_MEMBINDFNP(void, HardwareEthernetInterface, sndBuf, (unsigned)))
             .doc( "set the send socket buffer size in bytes"));
    consoleDir()
        .add("sndBuf", fty::Command(
                 SENF_MEMBINDFNP(unsigned, HardwareEthernetInterface, sndBuf, ()))
             .doc( "get the send socket buffer size in bytes"));
    consoleDir()
        .add("rcvBuf", fty::Command(
                 SENF_MEMBINDFNP(void, HardwareEthernetInterface, rcvBuf, (unsigned)))
             .doc( "set the receive socket buffer size in bytes"));
    consoleDir()
        .add("rcvBuf", fty::Command(
                 SENF_MEMBINDFNP(unsigned, HardwareEthernetInterface, rcvBuf, ()))
             .doc( "get the receive socket buffer size in bytes"));
    consoleDir()
        .add("qlen", fty::Command(
                 SENF_MEMBINDFNP(void, HardwareEthernetInterface, qlen, (unsigned)))
             .doc("set mmap read queue length in # of packets"));
    consoleDir()
        .add("qlen", fty::Command(
                 SENF_MEMBINDFNP(unsigned, HardwareEthernetInterface, qlen, () const))
             .doc("get mmap read queue length in # of packets"));
    consoleDir()
        .add("maxBurst", fty::Command(
                 SENF_MEMBINDFNP(void, HardwareEthernetInterface, maxBurst, (unsigned)))
             .doc("set max burst rate"));
    consoleDir()
        .add("maxBurst", fty::Command(
                 SENF_MEMBINDFNP(unsigned, HardwareEthernetInterface, maxBurst, () const))
             .doc("get max burst rate"));
    consoleDir()
        .add("pvid", fty::Command(
                 SENF_MEMBINDFNP(bool, HardwareEthernetInterface, pvid, (std::uint16_t)))
             .doc( "enables filtering for a specific PVID (VLAN ID must be 0...4095)"));
    consoleDir()
        .add("pvid", fty::Command(
                 SENF_MEMBINDFNP(std::uint16_t, HardwareEthernetInterface, pvid, () const))
             .doc( "report the currently configured PVID (-1 means none)"));


    console::provideDirectory(interfaceDir(),"by-device").add(dev_, fty::Link(consoleDir()));

    annotator_.id(id());

    if (enabled())
        init_sockets();
}

prefix_ std::string senf::emu::HardwareEthernetInterface::device()
    const
{
    return dev_;
}

prefix_ void senf::emu::HardwareEthernetInterface::v_mcAdd(MACAddress const & address)
{
    HardwareEthernetInterfaceNet::socket.protocol().mcAdd( dev_, address);
}

prefix_ void senf::emu::HardwareEthernetInterface::v_mcDrop(MACAddress const & address)
{
    HardwareEthernetInterfaceNet::socket.protocol().mcDrop( dev_, address);
}

prefix_ void senf::emu::HardwareEthernetInterface::init_sockets()
{
    std::string vlanDevice (device() + "." + senf::str(pvid_));

    if (!promisc() and pvid_ != std::uint16_t(-1)) {
        NetdeviceController nc (device());
        nc.addVLAN(pvid_);
        NetdeviceController ncv (vlanDevice);
        ncv.up();
    }
    
    ConnectedMMapPacketSocketHandle socket_ ((promisc() or pvid_ == std::uint16_t(-1) ? device() : vlanDevice),
                                             qlen_, SENF_EMU_MAXMTU);

    socket_.protocol().rcvbuf( rcvBufSize_);
    socket_.protocol().sndbuf( sndBufSize_);
    // socket_.protocol().sndLowat(SENF_EMU_MAXMTU);

    HardwareEthernetInterfaceNet::assignSockets(socket_);
    if (promisc()) {
        HardwareEthernetInterfaceNet::setupBPF(id(), true); // SRC only
    }
}

prefix_ void senf::emu::HardwareEthernetInterface::close_sockets()
{
    if (HardwareEthernetInterfaceNet::socket.valid())
        HardwareEthernetInterfaceNet::socket.close();

    if (!promisc() and pvid_ != std::uint16_t(-1)) {
        NetdeviceController nc (device());
        nc.delVLAN(pvid_);
    }

    HardwareEthernetInterfaceNet::assignSockets(socket);
}

prefix_ void senf::emu::HardwareEthernetInterface::v_enable()
{
    if (! enabled()) {
        ctrl_.up();
        init_sockets();
    }
}

prefix_ void senf::emu::HardwareEthernetInterface::v_disable()
{
    if (enabled()) {
        close_sockets();
        ctrl_.down();
    }
}

prefix_ bool senf::emu::HardwareEthernetInterface::v_enabled()
    const
{
    return ctrl_.isUp();
}

prefix_ void senf::emu::HardwareEthernetInterface::v_id(MACAddress const & mac)
{
    DisableInterfaceGuard guard (*this);
    ctrl_.hardwareAddress(mac);
    annotator_.id(id());
}

prefix_ senf::MACAddress senf::emu::HardwareEthernetInterface::v_id()
    const
{
    return ctrl_.hardwareAddress();
}

prefix_ bool senf::emu::HardwareEthernetInterface::v_promisc()
    const
{
    return ctrl_.promisc();
}

prefix_ void senf::emu::HardwareEthernetInterface::v_promisc(bool v)
{
    close_sockets();
    ctrl_.promisc(v);
    init_sockets();
    // inform the annotator about our promisc state (if promisc is on, all frames will be prepended with an AnnotationsPacket)
    annotator_.rawMode(v);
}

prefix_ unsigned senf::emu::HardwareEthernetInterface::v_mtu()
    const
{
    return ctrl_.mtu();
}

prefix_ void senf::emu::HardwareEthernetInterface::v_mtu(unsigned v)
{
    // nowadays, let's assume Gbit Interfaces ;)
    unsigned speed (1000);
    
    // this seems to fail for virtual interfaces (i.e. kvm)
    try {
        speed = ctrl_.speed();
    }
    catch(...) {};
    
    if ((speed < 1000) and (v > 1500)) {
        SENF_THROW_SYSTEM_EXCEPTION("EthernetController: interface/link in non-Jumbo mode. MTU > 1500 not allowed.");
        return;
    }
    
    ctrl_.mtu(v);
}

prefix_ void senf::emu::HardwareEthernetInterface::v_flushRxQueues()
{
    HardwareEthernetInterfaceNet::source.flush();
}

prefix_ void senf::emu::HardwareEthernetInterface::v_flushTxQueues()
{
//    HardwareEthernetInterfaceNet::sink.flush();
}

prefix_ void senf::emu::HardwareEthernetInterface::sndBuf(unsigned sndbuf)
{
    // need to cache sndBufSize_ so we can (re-)apply the value in v_enable()
    sndBufSize_ = std::max(sndbuf, 2048u);
    HardwareEthernetInterfaceNet::sndBuf( sndBufSize_);
}

prefix_ unsigned senf::emu::HardwareEthernetInterface::sndBuf()
{
    // need to cache sndBufSize_ so we can (re-)apply the value in v_enable()
    return sndBufSize_ = HardwareEthernetInterfaceNet::sndBuf();
}

prefix_ void senf::emu::HardwareEthernetInterface::rcvBuf(unsigned rcvbuf)
{
    // need to cache rcvBufSize_ so we can (re-)apply the value in v_enable()
    rcvBufSize_ = std::max(rcvbuf, 4096u);
    HardwareEthernetInterfaceNet::rcvBuf( rcvBufSize_);
}

prefix_ unsigned senf::emu::HardwareEthernetInterface::rcvBuf()
{
    // need to cache rcvBufSize_ so we can (re-)apply the value in v_enable()
    return rcvBufSize_ = HardwareEthernetInterfaceNet::rcvBuf();
}

prefix_ std::uint16_t senf::emu::HardwareEthernetInterface::pvid()
    const
{
    return pvid_;
}

prefix_ bool senf::emu::HardwareEthernetInterface::pvid(std::uint16_t p)
{
    if (p > 4095 and p != std::uint16_t(-1))
        return false;
    
    close_sockets();
    pvid_ = p;
    init_sockets();
    return true;
}


prefix_ unsigned senf::emu::HardwareEthernetInterface::qlen()
    const
{
    return qlen_;
}

prefix_ void senf::emu::HardwareEthernetInterface::qlen(unsigned qlen)
{
    if (qlen_ == qlen)
        return;
    // need to cache rxqlen_ so we can (re-)apply the value in v_enable()
    qlen_ = qlen;

    close_sockets();
    init_sockets();
}

prefix_ unsigned senf::emu::HardwareEthernetInterface::rxQueueDropped()
    const
{
    return HardwareEthernetInterfaceNet::socket.valid() ? HardwareEthernetInterfaceNet::socket.protocol().rxQueueDropped() : 0;
}

prefix_ unsigned senf::emu::HardwareEthernetInterface::maxBurst()
    const
{
    return source.maxBurst();
}

prefix_ void senf::emu::HardwareEthernetInterface::maxBurst(unsigned maxBurst)
{
    source.maxBurst(maxBurst);
}

prefix_ void senf::emu::HardwareEthernetInterface::dumpMmapStats(std::ostream & os)
{
    if (HardwareEthernetInterfaceNet::socket.valid()) {
        auto rs (HardwareEthernetInterfaceNet::socket.protocol().rxStats());
        os << "MMAP Rx stats: "
           << "received " << rs.received << ", "
           << "ignored "  << rs.ignored  << ". ";
        auto ts (HardwareEthernetInterfaceNet::socket.protocol().txStats());
        os << "MMAP Tx stats: "
           << "sent "        << ts.sent << ", "
           << "wrongFormat " << ts.wrongFormat << ", "
           << "overrun "     << ts.overrun << ", "
           << "dropped "     << ts.dropped << ". ";
        os << "DSQ stats: "
           << "dropped "     << sink.dropped() << std::endl;
    } else {
        os << "Socket closed. Not stats available." << std::endl;
    }
}

#ifdef SENF_DEBUG

prefix_ unsigned senf::emu::HardwareEthernetInterface::burstMax()
{
    return source.burstMax();
}

prefix_ unsigned senf::emu::HardwareEthernetInterface::sharedPackets()
{
    return source.sharedPackets();
}

#endif

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
