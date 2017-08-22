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
    \brief TunnelInterface non-inline non-template implementation */

#include "TunnelInterface.hh"

// Custom includes
#include <senf/Utils/Console/STLSupport.hh>
#include <senf/Utils/Console/Variables.hh>
#include <senf/Ext/NetEmu/AnnotationsPacket.hh>
#include <senf/Ext/NetEmu/config.hh>
#include "TunnelCtrlPacket.hh"
#include "TunnelHeaderPacket.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::emu::detail::TunnelIOHelper

template <class Controller>
prefix_ senf::emu::detail::TunnelIOHelper<Controller>::TunnelIOHelper(TunnelControllerBase & ctrl, TunnelInterfaceNet<Controller> const & tif)
    : ctrl_(ctrl), tunnelIface_(tif)
{}

template <class Controller>
prefix_ typename senf::emu::detail::TunnelIOHelper<Controller>::PacketType senf::emu::detail::TunnelIOHelper<Controller>::operator()(Handle & handle)
{
    auto pkt (ctrl_.readPacket(handle));
    if (!pkt)
        return pkt;
    
    if (SENF_UNLIKELY(tunnelIface_._annotationMode())) {
        return prependAnnotationsPacket(pkt);
    } else {
        return pkt;
    }
}

template <class Controller>
prefix_ bool senf::emu::detail::TunnelIOHelper<Controller>::operator()(Handle & handle, PacketType const & packet)
{
    if (SENF_LIKELY(packet.size() <= tunnelIface_.mtu() + 18))
        return ctrl_.writePacket(handle, packet.as<EthernetPacket>());
    
    return true;
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::TunnelInterfaceNet

template <class Controller>
prefix_ senf::emu::detail::TunnelInterfaceNet<Controller>::TunnelInterfaceNet(typename Controller::Interface & interface)
    : socket(senf::noinit), tunnelCtrl(interface),
      source(socket, TunnelIOHelper<Controller>(tunnelCtrl, *this)), sink(socket, TunnelIOHelper<Controller>(tunnelCtrl, *this)),
      netOutput(source.output), netInput(sink.input), mtu_(1500u), maxBurst_(48), promisc_(false), annotationMode_(false)
{
}

template <class Controller>
prefix_ void senf::emu::detail::TunnelInterfaceNet<Controller>::assignSocket(UDPv6ClientSocketHandle socket_)
{
    socket = socket_;
    source.handle(socket);
    source.maxBurst(maxBurst_);
    sink.handle(socket);
}

template <class Controller>
prefix_ unsigned senf::emu::detail::TunnelInterfaceNet<Controller>::rcvBuf()
{
    if (socket)
        return socket.protocol().rcvbuf();
    return 0;
}

template <class Controller>
prefix_ void senf::emu::detail::TunnelInterfaceNet<Controller>::rcvBuf(unsigned rcvbuf)
{
    if (socket)
        socket.protocol().rcvbuf(rcvbuf);
}

template <class Controller>
prefix_ unsigned senf::emu::detail::TunnelInterfaceNet<Controller>::sndBuf()
{
    if (socket)
        return socket.protocol().sndbuf();
    return 0;
}

template <class Controller>
prefix_ void senf::emu::detail::TunnelInterfaceNet<Controller>::sndBuf(unsigned sndbuf)
{
    if (socket)
        socket.protocol().sndbuf(sndbuf);
}

template <class Controller>
prefix_ unsigned senf::emu::detail::TunnelInterfaceNet<Controller>::mtu()
    const
{
    return mtu_;
}

template <class Controller>
prefix_ void senf::emu::detail::TunnelInterfaceNet<Controller>::mtu(unsigned v)
{
    v = std::max(576u, v);
    v = std::min(unsigned(SENF_EMU_MAXMTU - senf::EthernetPacketParser::fixed_bytes), v);
    mtu_ = v;
}

template <class Controller>
prefix_ unsigned senf::emu::detail::TunnelInterfaceNet<Controller>::maxBurst()
    const
{
    return source.maxBurst();
}

template <class Controller>
prefix_ void senf::emu::detail::TunnelInterfaceNet<Controller>::maxBurst(unsigned v)
{
    maxBurst_ = v;
    source.maxBurst(maxBurst_);
}

template <class Controller>
prefix_ void senf::emu::detail::TunnelInterfaceNet<Controller>::_promisc(bool p)
{
    promisc_ = p;
}

template <class Controller>
prefix_ bool senf::emu::detail::TunnelInterfaceNet<Controller>::_promisc()
    const
{
    return promisc_;
}

template <class Controller>
prefix_ void senf::emu::detail::TunnelInterfaceNet<Controller>::_annotationMode(bool a)
{
    annotationMode_ = a;
}

template <class Controller>
prefix_ bool senf::emu::detail::TunnelInterfaceNet<Controller>::_annotationMode()
    const
{
    return annotationMode_;
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::TunnelInterfaceBase

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

namespace senf { namespace emu { namespace tunnel {
    SENF_CONSOLE_REGISTER_ENUM( CapacityDirection, (FromClientToServer)(FromServerToClient) );
}}}

template <class Output, class Input>
prefix_ senf::emu::TunnelInterfaceBase::TunnelInterfaceBase(Output & output, Input & input,
        detail::TunnelControllerBase & ctrlBase, INet6SocketAddress const & address)
    : BidirectionalWiredInterface(output, input),
      ctrlBase_(ctrlBase), socketAddress_(address), ifaceIdFactory_( new SimpleInterfaceIdFactory()),
      id_(ifaceIdFactory_->getId()), isUp_(false), dev_("NetEMU-P2PTunnel")
{
    BidirectionalWiredInterface::init();
    HardwareInterface::init();

    namespace fty = console::factory;
    consoleDir()
        .add("timeout", fty::Command(
            SENF_MEMBINDFNP(ClockService::clock_type, TunnelInterfaceBase, timeout, () const)) );
    consoleDir()
        .add("timeout", fty::Command(
            SENF_MEMBINDFNP(void, TunnelInterfaceBase, timeout, (ClockService::clock_type))) );
    consoleDir()
        .add("info", fty::Command( &detail::TunnelControllerBase::dumpInfo, &ctrlBase_));
    consoleDir()
        .add("queue", qAlgorithm().consoleDir());
    consoleDir()
        .add("address", fty::Command(
            SENF_MEMBINDFNP(senf::INet6SocketAddress const &, TunnelInterfaceBase, address, () const)) );
    consoleDir()
        .add("fragmentationCount", fty::Command(&detail::TunnelControllerBase::fragmentationCount, &ctrlBase_));
}

prefix_ senf::emu::TunnelInterfaceBase::~TunnelInterfaceBase()
{
    ifaceIdFactory_->releaseId( id());
}

prefix_ senf::emu::detail::TunnelControllerBase & senf::emu::TunnelInterfaceBase::tunnelCtrlBase()
{
    return ctrlBase_;
}

prefix_ senf::emu::detail::TunnelControllerBase & senf::emu::TunnelInterfaceBase::tunnelCtrlBase()
    const
{
    return ctrlBase_;
}

prefix_ void senf::emu::TunnelInterfaceBase::qAlgorithm(senf::ppi::QueueingAlgorithm::ptr qAlgo)
{
    ctrlBase_.qAlgorithm( SENF_MOVE(qAlgo));
    consoleDir().add("queue", qAlgorithm().consoleDir());
}

prefix_ senf::ppi::QueueingAlgorithm & senf::emu::TunnelInterfaceBase::qAlgorithm()
    const
{
    return ctrlBase_.qAlgorithm();
}


prefix_ senf::INet6SocketAddress const & senf::emu::TunnelInterfaceBase::address()
    const
{
    return socketAddress_;
}

prefix_ senf::ClockService::clock_type senf::emu::TunnelInterfaceBase::timeout()
    const
{
    return ctrlBase_.timeout();
}

prefix_ void senf::emu::TunnelInterfaceBase::timeout(ClockService::clock_type t)
{
    ctrlBase_.timeout( t);
}

prefix_ boost::shared_ptr<senf::emu::InterfaceIdFactoryBase> senf::emu::TunnelInterfaceBase::ifaceIdFactory()
{
    return ifaceIdFactory_;
}

prefix_ void senf::emu::TunnelInterfaceBase::ifaceIdFactory(boost::shared_ptr<InterfaceIdFactoryBase> factory)
{
    DisableInterfaceGuard guard (*this);
    ifaceIdFactory_->releaseId( id_);
    ifaceIdFactory_ = factory;
    id_ = MACAddress::None;
    id( ifaceIdFactory_->getId());
}

prefix_ void senf::emu::TunnelInterfaceBase::isUp(bool up)
{
    isUp_ = up;
}

prefix_ bool senf::emu::TunnelInterfaceBase::v_enabled()
    const
{
    return isUp_;
}

prefix_ void senf::emu::TunnelInterfaceBase::v_id(MACAddress const & mac)
{
    DisableInterfaceGuard guard (*this);
    ifaceIdFactory_->releaseId( id_);
    id_ = mac;
}

prefix_ senf::MACAddress senf::emu::TunnelInterfaceBase::v_id()
    const
{
    return id_;
}

prefix_ void senf::emu::TunnelInterfaceBase::v_mcAdd(MACAddress const & address)
{
    throw senf::SystemException("multicast not supported on TunnelInterfaces", ENOSYS);
}

prefix_ void senf::emu::TunnelInterfaceBase::v_mcDrop(MACAddress const & address)
{
    throw senf::SystemException("multicast not supported on TunnelInterfaces", ENOSYS);
}

prefix_ boost::uint8_t senf::emu::TunnelInterfaceBase::v_linkTypeId()
    const
{
    return 5; // RADIUS network access server (NAS)-Port-Type for "virtual" (see RFC 2865)
}

prefix_ std::string const & senf::emu::TunnelInterfaceBase::v_device()
    const
{
    return dev_;
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::TunnelServerInterface

prefix_ senf::emu::TunnelServerInterface::TunnelServerInterface(INet4SocketAddress const & address)
    : TunnelServerInterface(INet6SocketAddress(INet6Address::from_inet4address(address.address()), address.port()))
{
}

prefix_ senf::emu::TunnelServerInterface::TunnelServerInterface(INet6SocketAddress const & address)
    : detail::TunnelServerInterfaceNet( *this),
      TunnelInterfaceBase(netOutput, netInput, tunnelCtrl, address),
      sndBufSize_(96*1024), rcvBufSize_(384*1024)
{
    namespace fty = console::factory;
    consoleDir()
        .add("sndBuf", fty::Command(
                 SENF_MEMBINDFNP(void, TunnelServerInterface, sndBuf, (unsigned)))
             .doc( "set the send socket buffer size in bytes"));
    consoleDir()
        .add("sndBuf", fty::Command(
                 SENF_MEMBINDFNP(unsigned, TunnelServerInterface, sndBuf, ()))
             .doc( "get the send socket buffer size in bytes"));
    consoleDir()
        .add("rcvBuf", fty::Command(
                 SENF_MEMBINDFNP(void, TunnelServerInterface, rcvBuf, (unsigned)))
             .doc( "set the receive socket buffer size in bytes"));
    consoleDir()
        .add("rcvBuf", fty::Command(
                 SENF_MEMBINDFNP(unsigned, TunnelServerInterface, rcvBuf, ()))
             .doc( "get the receive socket buffer size in bytes"));
    consoleDir()
        .add("fragmentationThreshold", fty::Command(
                 SENF_MEMBINDFNP(void, TunnelServerInterface, fragmentationThreshold, (MACAddress const &, unsigned)))
             .doc( "set the fragmentationThreshold for the specified client in bytes"));
    consoleDir()
        .add("fragmentationThreshold", fty::Command(
                 SENF_MEMBINDFNP(unsigned, TunnelServerInterface, fragmentationThreshold, (MACAddress const &) const))
             .doc( "get the fragmentationThreshold for the specified client in bytes"));
    consoleDir()
        .add("maxBurst", fty::Command(
                 SENF_MEMBINDFNP(void, TunnelServerInterface, maxBurst, (unsigned)))
             .doc( "set the maxBurst limit when receiving packets"));
    consoleDir()
        .add("maxBurst", fty::Command(
                 SENF_MEMBINDFNP(unsigned, TunnelServerInterface, maxBurst, () const))
             .doc( "get the maxBurst limit when receiving packets"));
}

prefix_ void senf::emu::TunnelServerInterface::v_enable()
{
    isUp(true);
    UDPv6ClientSocketHandle socket (address());
    socket.blocking(false); // make sure we operate in non-blocking mode to avoid queuing on kernel tx problems
    socket.protocol().mtuDiscovery(IP_PMTUDISC_DONT); 
//    socket.protocol().sndLowat(SENF_EMU_MAXMTU);
    assignSocket( socket);
    sndBuf(sndBufSize_);
    rcvBuf(rcvBufSize_);
}

prefix_ void senf::emu::TunnelServerInterface::v_disable()
{
    isUp(false);
    tunnelCtrl.terminateAllClients( sink.handle());
    assignSocket( UDPv6ClientSocketHandle(senf::noinit));
}

prefix_ unsigned senf::emu::TunnelServerInterface::capacity(MACAddress const & client, tunnel::CapacityDirection direction)
    const
{
    return tunnelCtrl.capacity(client, direction);
}

prefix_ unsigned senf::emu::TunnelServerInterface::fragmentationThreshold(MACAddress const & client)
    const
{
    return ((detail::TunnelServerController&)tunnelCtrlBase()).fragmentationThreshold(client);
}

prefix_ void senf::emu::TunnelServerInterface::fragmentationThreshold(MACAddress const & client, unsigned ft)
{
    ((detail::TunnelServerController&)tunnelCtrlBase()).fragmentationThreshold(client, ft);
}

prefix_ void senf::emu::TunnelServerInterface::sndBuf(unsigned sndbuf)
{
    // need to cache sndBufSize_ so we can (re-)apply the value in v_enable()
    sndBufSize_ = std::max(sndbuf, 2048u);
    TunnelInterfaceNet::sndBuf( sndBufSize_);
}

prefix_ unsigned senf::emu::TunnelServerInterface::sndBuf()
{
    // need to cache sndBufSize_ so we can (re-)apply the value in v_enable()
    return sndBufSize_ = TunnelInterfaceNet::sndBuf();
}

prefix_ void senf::emu::TunnelServerInterface::rcvBuf(unsigned rcvbuf)
{
    // need to cache rcvBufSize_ so we can (re-)apply the value in v_enable()
    rcvBufSize_ = std::max(rcvbuf, 4096u);
    TunnelInterfaceNet::rcvBuf( rcvBufSize_);
}

prefix_ unsigned senf::emu::TunnelServerInterface::rcvBuf()
{
    // need to cache rcvBufSize_ so we can (re-)apply the value in v_enable()
    return rcvBufSize_ = TunnelInterfaceNet::rcvBuf();
}

prefix_ unsigned senf::emu::TunnelServerInterface::v_mtu()
    const
{
    return TunnelInterfaceNet::mtu();
}

prefix_ void senf::emu::TunnelServerInterface::v_mtu(unsigned v)
{
    TunnelInterfaceNet::mtu(v);
}

prefix_ unsigned senf::emu::TunnelServerInterface::maxBurst()
    const
{
    return TunnelInterfaceNet::maxBurst();
}

prefix_ void senf::emu::TunnelServerInterface::maxBurst(unsigned v)
{
    TunnelInterfaceNet::maxBurst(v);
}

prefix_ void senf::emu::TunnelServerInterface::v_promisc(bool p)
{
    TunnelInterfaceNet::_promisc(p);
}

prefix_ bool senf::emu::TunnelServerInterface::v_promisc()
    const
{
    return TunnelInterfaceNet::_promisc();
}

prefix_ void senf::emu::TunnelServerInterface::v_annotationMode(bool a)
{
    TunnelInterfaceNet::_annotationMode(a);
}

prefix_ bool senf::emu::TunnelServerInterface::v_annotationMode()
    const
{
    return TunnelInterfaceNet::_annotationMode();
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::TunnelClientInterface

prefix_ senf::emu::TunnelClientInterface::TunnelClientInterface(INet4SocketAddress const & address)
    : TunnelClientInterface(INet6SocketAddress(INet6Address::from_inet4address(address.address()), address.port()))
{
}

prefix_ senf::emu::TunnelClientInterface::TunnelClientInterface(INet6SocketAddress const & address)
    : detail::TunnelClientInterfaceNet( *this),
      TunnelInterfaceBase(netOutput, netInput, tunnelCtrl, address),
      sndBufSize_(96*1024), rcvBufSize_(384*1024)
{
    namespace fty = console::factory;
    consoleDir()
        .add("established", fty::Command( &TunnelClientInterface::established, this));
    consoleDir()
        .add("serverAddress", fty::Command(
            SENF_MEMBINDFNP(void, TunnelClientInterface, serverAddress, (senf::INet6SocketAddress const &))) );
    consoleDir()
        .add("serverAddress", fty::Command(
            SENF_MEMBINDFNP(void, TunnelClientInterface, serverAddress, (senf::INet4SocketAddress const &))) );
    consoleDir()
        .add("serverAddress", fty::Command(
            SENF_MEMBINDFNP(senf::INet6SocketAddress const &, TunnelClientInterface, serverAddress, () const)) );
    consoleDir()
        .add("capacity", fty::Variable(capacity_));

    consoleDir()
        .add("sndBuf", fty::Command(
                 SENF_MEMBINDFNP(void, TunnelClientInterface, sndBuf, (unsigned)))
             .doc( "set the send socket buffer size in bytes"));
    consoleDir()
        .add("sndBuf", fty::Command(
                 SENF_MEMBINDFNP(unsigned, TunnelClientInterface, sndBuf, ()))
             .doc( "get the send socket buffer size in bytes"));
    consoleDir()
        .add("rcvBuf", fty::Command(
                 SENF_MEMBINDFNP(void, TunnelClientInterface, rcvBuf, (unsigned)))
             .doc( "set the receive socket buffer size in bytes"));
    consoleDir()
        .add("rcvBuf", fty::Command(
                 SENF_MEMBINDFNP(unsigned, TunnelClientInterface, rcvBuf, ()))
             .doc( "get the receive socket buffer size in bytes"));
    consoleDir()
        .add("fragmentationThreshold", fty::Command(
                 SENF_MEMBINDFNP(void, TunnelClientInterface, fragmentationThreshold, (unsigned)))
             .doc( "set the fragmentationThreshold for this tunnel in bytes"));
    consoleDir()
        .add("fragmentationThreshold", fty::Command(
                 SENF_MEMBINDFNP(unsigned, TunnelClientInterface, fragmentationThreshold, () const))
             .doc( "get the fragmentationThreshold for this tunnel in bytes"));
    consoleDir()
        .add("maxBurst", fty::Command(
                 SENF_MEMBINDFNP(void, TunnelClientInterface, maxBurst, (unsigned)))
             .doc( "set the maxBurst limit when receiving packets"));
    consoleDir()
        .add("maxBurst", fty::Command(
                 SENF_MEMBINDFNP(unsigned, TunnelClientInterface, maxBurst, () const))
             .doc( "get the maxBurst limit when receiving packets"));

    capacity_[tunnel::FromClientToServer] = 100000u;
    capacity_[tunnel::FromServerToClient] = 100000u;
}

prefix_ void senf::emu::TunnelClientInterface::v_enable()
{
    isUp(true);
    UDPv6ClientSocketHandle socket (address());
//    socket.connect(serverAddress()); // as a client: Only accept packets from our server 
    socket.blocking(false); // make sure we operate in non-blocking mode to avoid queuing on kernel tx problems
    socket.protocol().mtuDiscovery(IP_PMTUDISC_DONT); 
//    socket.protocol().sndLowat(SENF_EMU_MAXMTU);
    assignSocket(socket);
    sndBuf(sndBufSize_);
    rcvBuf(rcvBufSize_);
    tunnelCtrl.reset();
}

prefix_ void senf::emu::TunnelClientInterface::v_disable()
{
    isUp(false);
    assignSocket( UDPv6ClientSocketHandle(senf::noinit));
    tunnelCtrl.reset();
}

prefix_ void senf::emu::TunnelClientInterface::serverAddress(INet4SocketAddress const & address)
{
    serverAddress(INet6SocketAddress(INet6Address::from_inet4address(address.address()), address.port()));
}

prefix_ void senf::emu::TunnelClientInterface::serverAddress(INet6SocketAddress const & address)
{
    tunnelCtrl.serverAddress( address);
}

prefix_ senf::INet6SocketAddress const & senf::emu::TunnelClientInterface::serverAddress()
    const
{
    return tunnelCtrl.serverAddress();
}

prefix_ bool senf::emu::TunnelClientInterface::established()
    const
{
    return tunnelCtrl.established();
}

prefix_ unsigned senf::emu::TunnelClientInterface::capacity(tunnel::CapacityDirection direction)
    const
{
    tunnel::Capacity::const_iterator i (capacity_.find(direction));
    return i != capacity_.end() ? i->second : 0u;
}

prefix_ void senf::emu::TunnelClientInterface::capacity(tunnel::CapacityDirection direction, unsigned cap)
{
    capacity_[direction] = cap;
}

prefix_ unsigned senf::emu::TunnelClientInterface::fragmentationThreshold()
    const
{
    return ((detail::TunnelClientController&)tunnelCtrlBase()).fragmentationThreshold();
}

prefix_ void senf::emu::TunnelClientInterface::fragmentationThreshold(unsigned ft)
{
    ((detail::TunnelClientController&)tunnelCtrlBase()).fragmentationThreshold(ft);
}


prefix_ void senf::emu::TunnelClientInterface::sndBuf(unsigned sndbuf)
{
    // need to cache sndBufSize_ so we can (re-)apply the value in v_enable()
    sndBufSize_ = std::max(sndbuf, 2048u);
    TunnelInterfaceNet::sndBuf( sndBufSize_);
}

prefix_ unsigned senf::emu::TunnelClientInterface::sndBuf()
{
    // need to cache sndBufSize_ so we can (re-)apply the value in v_enable()
    return sndBufSize_ = TunnelInterfaceNet::sndBuf();
}

prefix_ void senf::emu::TunnelClientInterface::rcvBuf(unsigned rcvbuf)
{
    // need to cache rcvBufSize_ so we can (re-)apply the value in v_enable()
    rcvBufSize_ = std::max(rcvbuf, 4096u);
    TunnelInterfaceNet::rcvBuf( rcvBufSize_);
}

prefix_ unsigned senf::emu::TunnelClientInterface::rcvBuf()
{
    // need to cache rcvBufSize_ so we can (re-)apply the value in v_enable()
    return rcvBufSize_ = TunnelInterfaceNet::rcvBuf();
}

prefix_ unsigned senf::emu::TunnelClientInterface::v_mtu()
    const
{
    return TunnelInterfaceNet::mtu();
}

prefix_ void senf::emu::TunnelClientInterface::v_mtu(unsigned v)
{
    TunnelInterfaceNet::mtu(v);
}

prefix_ unsigned senf::emu::TunnelClientInterface::maxBurst()
    const
{
    return TunnelInterfaceNet::maxBurst();
}

prefix_ void senf::emu::TunnelClientInterface::maxBurst(unsigned v)
{
    TunnelInterfaceNet::maxBurst(v);
}

prefix_ void senf::emu::TunnelClientInterface::v_promisc(bool p)
{
    TunnelInterfaceNet::_promisc(p);
}

prefix_ bool senf::emu::TunnelClientInterface::v_promisc()
    const
{
    return TunnelInterfaceNet::_promisc();
}

prefix_ void senf::emu::TunnelClientInterface::v_annotationMode(bool a)
{
    TunnelInterfaceNet::_annotationMode(a);
}

prefix_ bool senf::emu::TunnelClientInterface::v_annotationMode()
    const
{
    return TunnelInterfaceNet::_annotationMode();
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
