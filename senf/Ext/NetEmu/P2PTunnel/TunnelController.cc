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
    \brief TunnelController non-inline non-template implementation */

#include "TunnelController.hh"

// Custom includes
#include <boost/foreach.hpp>
#include <senf/Utils/String.hh>
#include "TunnelHeaderPacket.hh"
#include "TunnelCtrlPacket.hh"
#include "TunnelInterface.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::TunnelControllerBase

const signed reSyncTresh = 1024;

prefix_ senf::emu::detail::TunnelControllerBase::TunnelControllerBase(TunnelInterfaceBase & interface)
    : timeout_(senf::ClockService::seconds(20)), interface_(interface), qAlgo_( new senf::ppi::NoneQueueingAlgorithm())
{}

prefix_ bool senf::emu::detail::TunnelControllerBase::isTunnelCtrlPacket(EthernetPacket const & eth)
{
    return eth->type_length() == EthOUIExtensionPacketType::etherType && eth.find<TunnelCtrlPacket>(senf::nothrow);
}

prefix_ senf::EthernetPacket senf::emu::detail::TunnelControllerBase::readPacket(Handle & handle)
{
    // use this as a trigger to flush our write queue
    flushQueue(handle);
    
    TunnelHeaderPacket thdr (TunnelHeaderPacket::create(senf::noinit));
    INet6SocketAddress addr;

    handle.readfrom(thdr.data(), addr, SENF_EMU_MAXMTU);

    if (SENF_UNLIKELY((thdr.size() < (TunnelHeaderPacket::Parser::fixed_bytes +  senf::EthernetPacketParser::fixed_bytes)) or 
        (thdr->reserved() != TunnelHeaderPacketType::reservedMagic)))
        return EthernetPacket();

    senf::EthernetPacket eth(thdr.next<senf::EthernetPacket>());

    if (SENF_UNLIKELY(isTunnelCtrlPacket(eth))) {
        try {
            v_handleCtrlPacket( eth, addr, handle);
        } catch (TruncatedPacketException &) {}
        return EthernetPacket();
    }

    // Ctrl Frames do not have a seqNo, so perform the here where we only see Data frames
    signed diff = v_processSequenceNumber(thdr, addr);

    eth.annotation<annotations::Interface>().value = interface_.id();
    eth.annotation<annotations::Timestamp>().fromSocketProtocol(handle.protocol());
    annotations::Quality & q (eth.annotation<annotations::Quality>());
    q.rssi  =  126; // one step below Ethernet to ensure native Ethernet Links yield a better SNR - this requires more thinking/integration
    q.noise = -128;
    q.snr   = q.rssi - q.noise;
    q.flags.frameLength = eth.size();

    if (SENF_LIKELY(diff == 1)) {
        // all is fine
    }
    else if ( diff == 0) {
        q.flags.frameDuplicate = true;
    }
    else if (diff > 1) {
        q.setLoss(diff - 1); 
    } else {
        // diff < 0 ==> reordering
        q.flags.frameDuplicate = true;
        q.setLoss(-diff); 
    }

    return eth;
}

prefix_ void senf::emu::detail::TunnelControllerBase::do_sendPkt(Handle & handle, senf::EthernetPacket & pkt)
{
    auto txInfo (v_getTxInfo(pkt));

    try{
        if (isTunnelCtrlPacket(pkt) or !fragmenter_.fragmentationRequired(pkt,txInfo.second)) {
            v_prependTHdr(pkt);
            handle.writeto(txInfo.first,pkt.prev().data());
        } else {
            std::vector<senf::EthernetPacket> frags;
            fragmenter_.fragmentFrame(pkt,txInfo.second, frags);
            for (auto & frag : frags) {
                v_prependTHdr(frag);
                handle.writeto(txInfo.first,frag.prev().data());
            }
        }
    }
    catch(...) {
    };
}

prefix_ bool senf::emu::detail::TunnelControllerBase::sendPkt(Handle & handle, MACAddress const & dstMAC, senf::EthernetPacket pkt)
{
    pkt.annotation<annotations::Interface>().value = dstMAC;
    
    // first, flush any possibly pending packets
    flushQueue(handle);

    if (qAlgo_->empty()) {
        if (handle.writeable()) {
            do_sendPkt(handle, pkt);
        } else {
            qAlgo_->enqueue(pkt);
        }
    } else {
        qAlgo_->enqueue(pkt);
    }
    
    return true;
}

prefix_ void senf::emu::detail::TunnelControllerBase::flushQueue(Handle & handle)
{
    while (handle.writeable()) {
        senf::Packet eth (qAlgo_->dequeue());
        if (eth) {
            do_sendPkt(handle, eth.as<EthernetPacket>());
        }
        else {
            break;
        }
    }
}

prefix_ bool senf::emu::detail::TunnelControllerBase::writePacket(Handle & handle, PacketType packet)
{
    return v_writePacket( handle, packet);
}

prefix_ void senf::emu::detail::TunnelControllerBase::sendCtrlPacket(
        Handle & handle, MACAddress const & dstMAC, boost::uint8_t code)
{
    TunnelCtrlPacket ctrlPacket (TunnelCtrlPacket::create());
    ctrlPacket->code() << code;
    sendCtrlPacket(handle, dstMAC, ctrlPacket);
}

prefix_ void senf::emu::detail::TunnelControllerBase::sendCtrlPacket(
        Handle & handle, MACAddress const & dstMAC, TunnelCtrlPacket ctrlPacket)
{
    EthernetPacket eth (EthernetPacket::create());
    eth->source( interface_.id())
       ->destination( dstMAC);
    EthOUIExtensionPacket ethOUIExt (EthOUIExtensionPacket::createAfter(eth));
    ethOUIExt.append(ctrlPacket);
    eth.finalizeAll();
    sendPkt( handle, dstMAC, eth);
}

prefix_ senf::ClockService::clock_type senf::emu::detail::TunnelControllerBase::timeout()
    const
{
    return timeout_;
}

prefix_ void senf::emu::detail::TunnelControllerBase::timeout(ClockService::clock_type t)
{
    timeout_ = t;
    v_timeoutChanged();
}

prefix_ senf::ppi::QueueingAlgorithm & senf::emu::detail::TunnelControllerBase::qAlgorithm()
    const
{
    return *qAlgo_;
}

prefix_ void senf::emu::detail::TunnelControllerBase::qAlgorithm(ppi::QueueingAlgorithm::ptr qAlgorithm)
{
    qAlgo_.reset(qAlgorithm.release());
}

prefix_ unsigned senf::emu::detail::TunnelControllerBase::fragmentationCount()
{
    return fragmenter_.fragmentationCount();
}


prefix_ void senf::emu::detail::TunnelControllerBase::dumpInfo(std::ostream & os)
    const
{
    os << "Id: " << interface_.id() << std::endl
       << "Enabled: " << (interface_.enabled() ? "yes" : "no") << std::endl
       << "Timeout: " << ClockService::in_seconds(timeout_) << " sec." << std::endl;
    v_dumpInfo(os);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::TunnelServerController

prefix_ senf::emu::detail::TunnelServerController::TunnelServerController(TunnelServerInterface & interface)
    : TunnelControllerBase(interface),
      clients_by_lastSeen_(clients_.get<ByLastSeen>()),
      clients_by_macAddr_(clients_.get<ByMACAddr>()),
      clients_by_inetAddr_(clients_.get<ByINetAddr>()),
      timer_( "senf::emu::TunnelServerController::timer", membind(&TunnelServerController::processTimeout, this))
{}

prefix_ bool senf::emu::detail::TunnelServerController::v_writePacket(Handle & handle, EthernetPacket & eth)
{
    MACAddress dst (eth->destination());
    if (SENF_UNLIKELY(dst.multicast())) {
        for (Clients_by_macAddr::iterator client = clients_by_macAddr_.begin(); client != clients_by_macAddr_.end(); client++) {
            sendPkt(handle, client->macAddr, eth.clone());
        }
        return true;
    } else {
        Clients_by_macAddr::iterator client (clients_by_macAddr_.find(dst));
        if (client != clients_by_macAddr_.end()) {
            return sendPkt(handle, client->macAddr, eth);
        }
    }
    return false;
}

prefix_ void senf::emu::detail::TunnelServerController::v_prependTHdr(EthernetPacket & eth)
{
    TunnelHeaderPacket thdr (TunnelHeaderPacket::createBefore(eth));

    if (SENF_LIKELY(!isTunnelCtrlPacket(eth))) {
        Clients_by_macAddr::iterator client (clients_by_macAddr_.find(eth.annotation<annotations::Interface>().value));
        if (client != clients_by_macAddr_.end()) {
            thdr->sequenceNumber() = client->txSeqNo;
            clients_by_macAddr_.modify( client, TunnelClient::incrTxSeqNo());
        }
    } else {
        thdr->sequenceNumber() = 0;
    }
    thdr.finalizeThis();
}

prefix_ std::pair<senf::INet6SocketAddress,unsigned> senf::emu::detail::TunnelServerController::v_getTxInfo(Packet const & eth)
    const
{
    Clients_by_macAddr::iterator client (clients_by_macAddr_.find(eth.annotation<annotations::Interface>().value));
    if (SENF_LIKELY(client != clients_by_macAddr_.end())) {
        return std::make_pair(client->inetAddr, client->fragmentationThreshold);
    }

    return std::make_pair(INet6SocketAddress(INet6Address::None,0), 0);
}


prefix_ signed senf::emu::detail::TunnelServerController::v_processSequenceNumber(TunnelHeaderPacket const & thdr, INet6SocketAddress const & srcAddr)
{
    Clients_by_inetAddr::iterator client (clients_by_inetAddr_.find(srcAddr));

    if (SENF_LIKELY(client != clients_by_inetAddr_.end())) {
        signed diff;
        if (SENF_UNLIKELY(client->rxSeqNo == 0xffffffff)) {
            diff = 1;
        } else {
            diff = TunnelHeaderPacketType::seqNoDiff(thdr->sequenceNumber(), client->rxSeqNo);
        }
        if ((diff > 0) or (diff < -reSyncTresh)) {
            clients_by_inetAddr_.modify( client, TunnelClient::updateRxSeqNo(thdr->sequenceNumber()));
            if (diff < -reSyncTresh) {
                clients_by_inetAddr_.modify( client, TunnelClient::incReSyncs());
            }
        }
        return diff;
    }

    return 1;
}

prefix_ void senf::emu::detail::TunnelServerController::v_handleCtrlPacket(
        EthernetPacket const & ethPacket, INet6SocketAddress const & srcAddr, Handle & handle)
{
    TunnelCtrlPacket ctrlPacket (ethPacket.find<TunnelCtrlPacket>());
    Clients_by_macAddr::iterator clientByMAC (clients_by_macAddr_.find(ethPacket->source()));
    Clients_by_inetAddr::iterator clientByINet (clients_by_inetAddr_.find(srcAddr));
    unsigned responseCode (ctrlPacket->code() + 1); // ==0 => ignore; >0 => code
    bool updateLastSeen (false);
    bool updateINetAddr (false);

    switch (ctrlPacket->code()) {
    case TunnelCtrlPacketParser::SetupRequest:
        if (! ctrlPacket->has_capacity()) {
            // no capacity?! => reject
            responseCode = TunnelCtrlPacketParser::SetupReject;
            break;
        }
        if (clientByMAC == clients_by_macAddr_.end()) {
            if (clientByINet == clients_by_inetAddr_.end()) {
                // src mac and src ip/port not know => we have a new client
                clientByMAC = clients_by_macAddr_.insert( TunnelClient(ethPacket->source(), srcAddr)).first;
                updateLastSeen = true;
            } else {
                // unknown mac but known ip/port => maybe mac changed?! reject
                responseCode = TunnelCtrlPacketParser::SetupReject;
            }
        } else {
            if (clientByINet == clients_by_inetAddr_.end())
                // known mac but unknown ip/port => update ip/port
                updateINetAddr = true;
            // else know mac and known ip/port => ok.
            updateLastSeen = true;
        }
        clients_by_macAddr_.modify( clientByMAC, TunnelClient::updateCapacity(
                tunnel::FromClientToServer, ctrlPacket->capacity().fromClientToServer()) );
        clients_by_macAddr_.modify( clientByMAC, TunnelClient::updateCapacity(
                tunnel::FromServerToClient, ctrlPacket->capacity().fromServerToClient()) );
        break;

    case TunnelCtrlPacketParser::EchoRequest:
        if (clientByMAC == clients_by_macAddr_.end()) {
            // unknown clients don't get a EchoReply
            responseCode = 0; // ignore
        } else {
            if (clientByINet == clients_by_inetAddr_.end()) {
                // known mac but unknown ip/port => update ip/port
                updateINetAddr = true;
            }
            // else know mac and known ip/port => ok.
            updateLastSeen = true;
        }
        break;

    case TunnelCtrlPacketParser::TerminateRequest:
        if (clientByMAC != clients_by_macAddr_.end() && clientByINet != clients_by_inetAddr_.end())
            // know mac an known ip/port => remove client
            clients_by_macAddr_.erase( clientByMAC);
        else
            // unknown clients don't get an Ack
            responseCode = 0; // ignore
        break;

    case TunnelCtrlPacketParser::TerminateAck:
        break;

    case TunnelCtrlPacketParser::SetupAck:
    case TunnelCtrlPacketParser::SetupReject:
    case TunnelCtrlPacketParser::EchoReply:
        SENF_LOG((senf::log::IMPORTANT)("invalid TunnelCtrlPacket received: " << unsigned(ctrlPacket->code())));
        break;
    }

    if (updateLastSeen) {
        clients_by_macAddr_.modify( clientByMAC, TunnelClient::updateLastSeen());
        resetTimer();
    }
    if (updateINetAddr)
        clients_by_macAddr_.modify( clientByMAC, TunnelClient::updateInetAddr(srcAddr));

    if (responseCode != 0)
        sendCtrlPacket( handle, ethPacket->source(), responseCode);
}

prefix_ void senf::emu::detail::TunnelServerController::processTimeout()
{
    SENF_ASSERT( !clients_.empty(), "empty client map in TunnelServerController::processTimeout");

    Clients_by_lastSeen::iterator client (clients_by_lastSeen_.begin());
    SENF_LOG((senf::log::MESSAGE)("timeout for tunnel client ("
            << client->macAddr << ", " << client->inetAddr << ") last seen " <<
            ClockService::in_seconds(scheduler::now() - client->lastSeen) << " sec."));
    clients_by_lastSeen_.erase( client);

    resetTimer();
}

prefix_ void senf::emu::detail::TunnelServerController::resetTimer()
{
    if (clients_.empty())
        timer_.disable();
    else
        timer_.timeout(clients_by_lastSeen_.begin()->lastSeen + timeout());
}

prefix_ void senf::emu::detail::TunnelServerController::terminateAllClients(Handle handle)
{
    BOOST_FOREACH( TunnelClient const & client, clients_) {
        sendCtrlPacket( handle, client.macAddr, TunnelCtrlPacketParser::TerminateRequest);
    }
}

namespace {
    unsigned get(senf::emu::tunnel::Capacity const & capacity, senf::emu::tunnel::CapacityDirection direction) {
        senf::emu::tunnel::Capacity::const_iterator i (capacity.find(direction));
        return i != capacity.end() ? i->second : 0;
    }
}

prefix_ unsigned senf::emu::detail::TunnelServerController::capacity(MACAddress const & clientAddr, tunnel::CapacityDirection direction)
    const
{
    Clients_by_macAddr::const_iterator client (clients_by_macAddr_.find(clientAddr));
    return client != clients_by_macAddr_.end() ? get(client->capacity, direction) : 0;
}

prefix_ void senf::emu::detail::TunnelServerController::fragmentationThreshold(MACAddress const & clientAddr, unsigned ft)
{
    if (ft == 0)
        ft = 1280u;

    Clients_by_macAddr::const_iterator client (clients_by_macAddr_.find(clientAddr));
    if ( client != clients_by_macAddr_.end())
        clients_by_macAddr_.modify( client, TunnelClient::updateFragmentationThreshold(ft - TunnelControllerBase::TunnelOverhead));
}

prefix_ unsigned senf::emu::detail::TunnelServerController::fragmentationThreshold(MACAddress const & clientAddr)
    const
{
    Clients_by_macAddr::const_iterator client (clients_by_macAddr_.find(clientAddr));
    return client != clients_by_macAddr_.end() ? (client->fragmentationThreshold + TunnelControllerBase::TunnelOverhead) : 0;
}


prefix_ void senf::emu::detail::TunnelServerController::v_dumpInfo(std::ostream & os)
    const
{
    os << "Clients:";
    if (clients_.empty()) {
        os << " none" << std::endl;
        return;
    }
    os << std::endl;
    boost::format fmtClient ("%-30s  %-17s  %4d  %9d %4s  %18d %18d %5x %5x %7d");
    os << fmtClient % "address" % "MAC address" % "FragThesh" % "last seen" %"" % "capacity_to_client" % "capacity_from_client" % "TxSeq" % "RxSeq" % "ReSyncs"<< std::endl;
    BOOST_FOREACH( TunnelClient const & client, clients_) {
        os << fmtClient
              % senf::str(client.inetAddr)
              % senf::str(client.macAddr)
              % senf::str(client.fragmentationThreshold + TunnelControllerBase::TunnelOverhead)
              % ClockService::in_seconds(scheduler::now() - client.lastSeen) % "sec."
              % get(client.capacity, tunnel::FromServerToClient)
              % get(client.capacity, tunnel::FromClientToServer)
              % client.txSeqNo
              % client.rxSeqNo
              % client.reSyncs
           << std::endl;
    }
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::TunnelClientController

prefix_ senf::emu::detail::TunnelClientController::TunnelClientController(TunnelClientInterface & interface)
    : TunnelControllerBase(interface),
      interface_(interface), established_(false),
      timer_( "TunnelClientController::timer", membind(&TunnelClientController::processTimeout, this)),
      serverLastSeen_(0),
      txSeqNo_(0),
      rxSeqNo_(0xffffffff),
      setupRequests_(0),
      reSyncs_(0)
{
    v_timeoutChanged();
    fragmentationThreshold(0);
}

prefix_ signed senf::emu::detail::TunnelClientController::v_processSequenceNumber(TunnelHeaderPacket const & thdr, INet6SocketAddress const & srcAddr)
{
    signed diff;
    if (SENF_UNLIKELY(rxSeqNo_ == 0xffffffff)) {
        diff = 1;
    } else {
        diff = TunnelHeaderPacketType::seqNoDiff(thdr->sequenceNumber(), rxSeqNo_);
    }
    if ((diff > 0) or (diff < -reSyncTresh)) {
        rxSeqNo_ = thdr->sequenceNumber();
        if (diff < -reSyncTresh) {
            reSyncs_++;
        }
    }
    return diff;
}

prefix_ void senf::emu::detail::TunnelClientController::v_handleCtrlPacket(
        EthernetPacket const & ethPacket, INet6SocketAddress const & srcAddr, Handle & handle)
{
    if (srcAddr != serverAddress()) {
        SENF_LOG(("skipping control Packet from unknown server " << srcAddr));
        return; // we only accept control packets from the configured tunnel server
    }
    serverLastSeen_ = senf::scheduler::now();
    TunnelCtrlPacket ctrlPacket (ethPacket.find<TunnelCtrlPacket>());
    switch (ctrlPacket->code()) {
    case TunnelCtrlPacketParser::SetupAck:
        established_ = true;
        serverMAC_ = ethPacket->source();
        break;

    case TunnelCtrlPacketParser::SetupReject:
        SENF_LOG((senf::log::IMPORTANT)("Tunnel setup rejected."));
    case TunnelCtrlPacketParser::TerminateRequest:
    case TunnelCtrlPacketParser::TerminateAck:
        SENF_LOG((senf::log::IMPORTANT)("Tunnel terminated."));
        established_ = false;
        serverMAC_ = MACAddress::None;
        break;

    case TunnelCtrlPacketParser::EchoReply:
        break;

    case TunnelCtrlPacketParser::EchoRequest:
    case TunnelCtrlPacketParser::SetupRequest:
        SENF_LOG((senf::log::IMPORTANT)("invalid TunnelCtrlPacket received: " << unsigned(ctrlPacket->code())));
        break;
    }
}



prefix_ bool senf::emu::detail::TunnelClientController::v_writePacket(Handle & handle, EthernetPacket & eth)
{
    if (SENF_LIKELY(established_)) {
        return sendPkt( handle, serverMAC_, eth);
    }

    return false;
}

prefix_ void senf::emu::detail::TunnelClientController::v_prependTHdr(EthernetPacket & eth)
{
    TunnelHeaderPacket thdr (TunnelHeaderPacket::createBefore(eth));
    if (SENF_LIKELY(!isTunnelCtrlPacket(eth))) {
        thdr->sequenceNumber() = txSeqNo_;
        txSeqNo_ = (txSeqNo_ + 1) % 0x20000;
    } else {
        thdr->sequenceNumber() = 0;
    }
    thdr.finalizeThis();
}

prefix_ std::pair<senf::INet6SocketAddress,unsigned> senf::emu::detail::TunnelClientController::v_getTxInfo(Packet const & eth)
    const
{
    return std::make_pair(serverINet_, fragmentationThreshold_);
}

prefix_ void senf::emu::detail::TunnelClientController::processTimeout()
{
    timer_.timeout( ClockService::now() + echoInterval_);
    if (established_) {
        if (scheduler::now() - serverLastSeen_ < timeout()) {
            sendCtrlPacket( interface_.socket, serverMAC_, TunnelCtrlPacketParser::EchoRequest);
            return;
        }
        // connection time out
        established_ = false;
        serverMAC_ = MACAddress::None;
        setupRequests_ = 0;
        txSeqNo_ = 0;
        rxSeqNo_ = 0xFFFFFFFF;
        reSyncs_ = 0;
        fragmentationThreshold(0);
    }
    if (serverINet_ && interface_.enabled())
        sendSetupRequest();
}

prefix_ void senf::emu::detail::TunnelClientController::sendSetupRequest()
{
    TunnelCtrlPacket setupRequest (TunnelCtrlPacket::create());
    setupRequest->add_capacity();
    setupRequest->capacity().fromClientToServer() << interface_.capacity(tunnel::FromClientToServer);
    setupRequest->capacity().fromServerToClient() << interface_.capacity(tunnel::FromServerToClient);
    sendCtrlPacket( interface_.socket, MACAddress::Broadcast, setupRequest);
    setupRequests_++;
}

prefix_ bool senf::emu::detail::TunnelClientController::established()
    const
{
    return established_;
}

prefix_ void senf::emu::detail::TunnelClientController::reset()
{
    if (interface_.enabled()) {
        established_ = false;
        processTimeout();
    } else {
        timer_.timeout( senf::ClockService::clock_type(0), false);
        if (established_) {
            established_ = false;
            sendCtrlPacket( interface_.socket, serverMAC_, TunnelCtrlPacketParser::TerminateRequest);
        }
    }
    serverMAC_ = MACAddress::None;
    serverLastSeen_ = ClockService::clock_type(0);
    setupRequests_ = 0;
    txSeqNo_ = 0;
    rxSeqNo_ = 0xFFFFFFFF;
    reSyncs_ = 0;
    fragmentationThreshold(0);
}

prefix_ void senf::emu::detail::TunnelClientController::serverAddress(INet6SocketAddress const & address)
{
    serverINet_ = address;
    reset();
}

prefix_ senf::INet6SocketAddress const & senf::emu::detail::TunnelClientController::serverAddress()
    const
{
    return serverINet_;
}

prefix_ void senf::emu::detail::TunnelClientController::fragmentationThreshold(unsigned ft)
{
    if (ft == 0)
        ft = 1280u;

    fragmentationThreshold_ = ft - TunnelControllerBase::TunnelOverhead;
}

prefix_ unsigned senf::emu::detail::TunnelClientController::fragmentationThreshold()
    const
{
    return fragmentationThreshold_ + TunnelControllerBase::TunnelOverhead;
}

prefix_ void senf::emu::detail::TunnelClientController::v_timeoutChanged()
{
    echoInterval_ = senf::ClockService::milliseconds(1000);
}

prefix_ void senf::emu::detail::TunnelClientController::v_dumpInfo(std::ostream & os)
    const
{
    os << "Echo interval: " << ClockService::in_seconds(echoInterval_) << " sec." << std::endl
       << "Connection established: " << (established_ ? "yes" : "no") << std::endl
       << "Server address: " << serverINet_ << std::endl
       << "Server MACAddress: " << serverMAC_ << std::endl
       << "Server last seen: "
           << (serverLastSeen_ ? senf::str(ClockService::in_seconds(scheduler::now() - serverLastSeen_)) : "none")
       << " sec." << std::endl
       << "fragThresh " << fragmentationThreshold() << std::endl
       << "setupREQs sent " << setupRequests_ << std::endl
       << "txSeqNo: 0x" << std::hex << txSeqNo_ << ", rxSeqno 0x" << rxSeqNo_ << std::dec << ", reSyncs " << reSyncs_ << std::endl;
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
