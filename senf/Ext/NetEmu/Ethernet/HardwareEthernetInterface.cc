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
      annotatorRx_(true, true), annotatorTx_(false, true),
      netOutput (annotatorRx_.output), netInput (annotatorTx_.input)
{
    senf::ppi::connect(source.output, annotatorRx_.input);
    senf::ppi::connect(annotatorTx_.output, sink.input);
}

prefix_ void senf::emu::detail::HardwareEthernetInterfaceNet::assignSockets(ConnectedMMapPacketSocketHandle & socket_)
{
    source.handle(socket_);
    sink.handle(socket_);

    if (!socket_) {
        if(socket) {
            socket.close();
        }
    } else {
        socket = socket_;
    }
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

prefix_ senf::emu::HardwareEthernetInterface::HardwareEthernetInterface(std::string const & dev)
    : EthernetInterface (netOutput, netInput), dev_ (dev), ctrl_ (dev_),
      rcvBufSize_ (1024), sndBufSize_ (96*1024), qlen_ (512), pvid_(VLanId::None), accessMode_(false)
{
    EthernetInterface::init();
    HardwareInterface::init();

    namespace fty = console::factory;

    consoleDir()
        .add("mmapStats", fty::Command(&HardwareEthernetInterface::dumpMmapStats, this)
        .doc("dumps and resets the MMAP RX/TX statistics"));
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
                 SENF_MEMBINDFNP(bool, HardwareEthernetInterface, pvid, (VLanId const &, bool)))
             .doc( "enables filtering for a specific PVID (VLAN ID must be 0...4095)"));
    consoleDir()
        .add("pvid", fty::Command(
                 SENF_MEMBINDFNP(VLanId const &, HardwareEthernetInterface, pvid, () const))
             .doc( "report the currently configured PVID (-1 means none)"));


    console::provideDirectory(interfaceDir(),"by-device").add(device(), fty::Link(consoleDir()));

    initialId_ = id();
    annotatorRx_.id(initialId_);

    if (ctrl_.isUp())
        init_sockets();
}

prefix_ senf::emu::HardwareEthernetInterface::~HardwareEthernetInterface()
{
    try {
        console::provideDirectory(interfaceDir(),"by-device").remove(device());
    } catch (...) {};
}

prefix_ std::string const & senf::emu::HardwareEthernetInterface::v_device()
    const
{
    return dev_;
}

prefix_ void senf::emu::HardwareEthernetInterface::v_mcAdd(MACAddress const & address)
{
    if (HardwareEthernetInterfaceNet::socket.valid())
        HardwareEthernetInterfaceNet::socket.protocol().mcAdd( dev_, address);
}

prefix_ void senf::emu::HardwareEthernetInterface::v_mcDrop(MACAddress const & address)
{
    if (HardwareEthernetInterfaceNet::socket.valid())
        HardwareEthernetInterfaceNet::socket.protocol().mcDrop( dev_, address);
}

prefix_ void senf::emu::HardwareEthernetInterface::init_sockets()
{
    std::string vlanDevice (device() + "." + senf::str(pvid_));

    if (!promisc() and pvid_) {
        // if there exists a VLAN interface, remove it first
        try {
            ctrl_.delVLAN(pvid_.id());
        }
        catch (...) {
        }
        ctrl_.addVLAN(pvid_.id());
        NetdeviceController(vlanDevice).up();
    }
    
    ConnectedMMapPacketSocketHandle socket_ (((promisc() or !pvid_) ? device() : vlanDevice),
                                             qlen_, SENF_EMU_MAXMTU);

    socket_.protocol().rcvbuf( rcvBufSize_);
    socket_.protocol().sndbuf( sndBufSize_);
    // socket_.protocol().sndLowat(SENF_EMU_MAXMTU);

    HardwareEthernetInterfaceNet::assignSockets(socket_);
    if (promisc()) {
        HardwareEthernetInterfaceNet::setupBPF(id(), true); // SRC only
    }

    if (promisc() and pvid_) {
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

    // switch to promisc rx method, which works around possibily misconfigured VLAN offloading 
    annotatorRx_.promisc(promisc());
}

prefix_ void senf::emu::HardwareEthernetInterface::close_sockets()
{
    try {
        if (!promisc() and pvid_) {
            ctrl_.delVLAN(pvid_.id());
        }
    }
    catch (...) {
    }

    ConnectedMMapPacketSocketHandle skt (senf::noinit);
    HardwareEthernetInterfaceNet::assignSockets(skt);
}

prefix_ void senf::emu::HardwareEthernetInterface::v_enable()
{
    if (!enabled()) {
        // we might have a new ifindex_ (i.e. after USB Ethernet unplug/plug)
        ctrl_.reset(device());
        // did our ID change ?  
        if (initialId_ != id()) {
            // if so, reset it to the initial Id (i.e. USB Ethernet)
            SENF_LOG((senf::log::IMPORTANT)("Id (MAC Address) of device " << device() << " has changed to " << id() << ". Resetting it back to " << initialId_));
            // iface must be down before changing Id
            ctrl_.down();
            // reset to old Id here
            id(initialId_);
        }
        // make sure the interface is 'up'
        ctrl_.up();
        // open the MMAP sockets
        init_sockets();
    }
}

prefix_ void senf::emu::HardwareEthernetInterface::v_disable()
{
    if (enabled()) {
        close_sockets();
        try {
            ctrl_.down();
        }
        catch (...) {
        }            
    }
}

prefix_ bool senf::emu::HardwareEthernetInterface::v_enabled()
    const
{
    return HardwareEthernetInterfaceNet::socket.valid();
}

prefix_ void senf::emu::HardwareEthernetInterface::v_id(MACAddress const & mac)
{
    DisableInterfaceGuard guard (*this);
    ctrl_.hardwareAddress(mac);
    annotatorRx_.id(id());
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

prefix_ void senf::emu::HardwareEthernetInterface::v_promisc(bool p)
{
    close_sockets();
    ctrl_.promisc(p);
    init_sockets();
}

prefix_ bool senf::emu::HardwareEthernetInterface::v_annotationMode()
    const
{
    return annotatorRx_.annotate();
}

prefix_ void senf::emu::HardwareEthernetInterface::v_annotationMode(bool a)
{
    annotatorRx_.annotate(a);
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
    HardwareEthernetInterfaceNet::sink.flush();
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

prefix_ bool senf::emu::HardwareEthernetInterface::pvid(VLanId const & p, bool accessMode)
{
    if (!accessMode and p.stag())
        return false;

    close_sockets();
    pvid_ = p;
    accessMode_ = accessMode;
    init_sockets();
    return true;
}

prefix_ std::uint32_t senf::emu::HardwareEthernetInterface::vlanMismatchRx()
{
    return HardwareEthernetInterfaceNet::annotatorRx_.vlanMismatch();
}

prefix_ std::uint32_t senf::emu::HardwareEthernetInterface::vlanMismatchTx()
{
    return HardwareEthernetInterfaceNet::annotatorTx_.vlanMismatch();
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

    if (enabled()) {
        close_sockets();
        init_sockets();
    }
}

prefix_ std::pair<unsigned,unsigned> senf::emu::HardwareEthernetInterface::rxDropped()
{
    return source.dropped();
}

prefix_ std::tuple<unsigned,unsigned,unsigned> senf::emu::HardwareEthernetInterface::txDropped()
{
    return sink.dropped();
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
        os << "MMAP Rx (fd=" << socket.protocol().fd() << ", avail " << socket.protocol().available() << ") stats: "; rs.dump(os);
        auto ts (HardwareEthernetInterfaceNet::socket.protocol().txStats());
        os << " MMAP Tx (fd=" << socket.protocol().fd() << ") stats: "; ts.dump(os);
    } else {
        os << "Socket closed. No stats available.";
    }
    os << std::endl;
}

prefix_ bool senf::emu::HardwareEthernetInterface::isDead()
    const
{
    if (HardwareEthernetInterfaceNet::socket)
        return socket.protocol().interfaceDead();
    return false;  // do not call us if the iface is not active
}


#ifdef SENF_DEBUG

prefix_ unsigned senf::emu::HardwareEthernetInterface::burstMax()
{
    return source.burstMax();
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
