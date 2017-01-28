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
    \brief EmulatedInterface non-inline non-template implementation */

#include "EmulatedInterface.hh"

// Custom includes
#include "Annotations.hh"
#include <senf/Packets/80211Bundle/WLANPacket.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

std::uint32_t senf::emu::EmulatedInterface::emuInterfaceIndex = 0;

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::EmulatedInterfaceReceiveFilter

prefix_ void senf::emu::detail::EmulatedInterfaceReceiveFilter::request()
{
    PacketHeader p (input());
    EthernetPacket e (p.find<EthernetPacket>(senf::nothrow));

    Interface & iface (receiver_.EmulatedReceiver::basei::interface());

    if (e) {
        // Drop self-looped packets
        if (e->source() == iface.id())
            return;

        // Drop all packets not addressed to us
        if (! iface.receiver().promisc() ) {
            MACAddress dst (e->destination());
            if (! dst.multicast() && dst != iface.id())
                return;
        }
    } else { //non-data frames (i.e no Ethernet header in stack)
        // TODO: make this more generic:
        // Currently this covers only WLAN non-data frames.
        // Are more generic solution might be needed if we want to
        // have similar mgt frames for DVB (or WiMAX) as well
        WLANPacket_MgtFrame w (p.find<WLANPacket_MgtFrame>(senf::nothrow));

        if (! w)
            return;
        // Drop self-looped packets
        if (w->sourceAddress() == iface.id())
            return;
    }

    // Cry loudly if someone else sends with our own node id
    if (Interface::nodeId() == p->nodeId() && iface.index() == p->interfaceIndex())
        SENF_LOG((senf::log::CRITICAL)("Duplicate nodeId,ifIndex:  "
                                       << p->nodeId() << "," << p->interfaceIndex()));

    p.annotation<annotations::Timestamp>().fromSocketProtocol(receiver_.EmulatedReceiver::emui::interface().emulationSocket().protocol());  
    p.annotation<annotations::Interface>().value = iface.id();

    output(p);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::EmulatedInterfaceTransmitFilter

prefix_ void senf::emu::detail::EmulatedInterfaceTransmitFilter::request()
{
    Packet p (input());
    if (mtu_) {
        Packet::size_type sz (transmitter_.v_emulatedPayloadSize(p));
        if (sz > mtu_) {
            SENF_LOG((senf::log::IMPORTANT)(
                         "Dropping emulated packet: emulated packet size " << sz
                         << " exceeds device MTU " << mtu_));
            return;
        }
    }

    timeval tv;
    gettimeofday( &tv, NULL);
    PacketHeader ph (PacketHeader::createBefore(p));
    ph->timestamp() = (tv.tv_sec * 1000000000ULL + tv.tv_usec * 1000ULL);
    ph->sequenceNumber() = seqNr_++;
    ph->nodeId() = Interface::nodeId();
    ph->interfaceIndex() = transmitter_.basei::interface().index();
    ph.finalizeThis();
    output(ph);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::DroppedPacketChecker

prefix_ senf::emu::detail::DroppedPacketChecker::DroppedPacketChecker()
    : droppedPackets_ (0u), lastDroppedPackets_ (0u), factor_ (0.0f)
{
    registerEvent(statisticsTimer_, &DroppedPacketChecker::statisticsUpdate);
}

prefix_ void senf::emu::detail::DroppedPacketChecker::v_handlePacket(PacketHeader const & p)
{
    boost::uint32_t seqNo (p->sequenceNumber());
    SeqNoMap::iterator i (seqNos_.find(std::make_pair(p->nodeId(),p->interfaceIndex())));
    if (i == seqNos_.end()) {
        seqNos_.insert(
            std::make_pair(
                std::make_pair(p->nodeId().value(), p->interfaceIndex().value()),
                seqNo ));
        return;
    }
    if (seqNo > i->second+1) {
        SENF_LOG((senf::log::IMPORTANT)
                 ("Dropped packet from nodeId,ifIndex " <<
                  i->first.first << "," << i->first.second << " "
                  "detected at sequence number " << seqNo));
        SENF_LOG((senf::log::NOTICE)(dumpPacketInfo(p)));
        ++ droppedPackets_;
        ++ lastDroppedPackets_;
    }
    else if (seqNo <= i->second ) {
        SENF_LOG((senf::log::IMPORTANT)
                 ("Packet reversal or duplicate packet from nodeId,ifIndex " <<
                  i->first.first << "," << i->first.second << " "
                  "detected at sequence number " << seqNo));
    }
    i->second = seqNo;
}

prefix_ void senf::emu::detail::DroppedPacketChecker::
startStatistics(ClockService::clock_type pollInterval)
{
    statisticsTimer_.interval(pollInterval);
    factor_ = double(ClockService::in_nanoseconds(pollInterval)) /
        double(ClockService::in_nanoseconds(ClockService::seconds(1)));
}

prefix_ void senf::emu::detail::DroppedPacketChecker::statisticsUpdate()
{
    signals.droppedPacketsPerSecond(lastDroppedPackets_/factor_);
    signals.numPeers(seqNos_.size());
    lastDroppedPackets_ = 0;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::DelayTimeChecker

prefix_
senf::emu::detail::DelayTimeChecker::DelayTimeChecker(ClockService::clock_type maxDelay)
    : maxDelay_ (maxDelay), delayedPackets_ (0u), maxPacketDelay_ (0u), minDelayStat_ (0u),
      totalDelayStat_ (0u), maxDelayStat_ (0u), nPackets_ (0u)
{
    registerEvent(statisticsTimer_, &DelayTimeChecker::statisticsUpdate);
}

prefix_ void
senf::emu::detail::DelayTimeChecker::startStatistics(ClockService::clock_type pollInterval)
{
    statisticsTimer_.interval(pollInterval);
}

prefix_ void senf::emu::detail::DelayTimeChecker::v_handlePacket(PacketHeader const & p)
{
    ClockService::clock_type sendTime (p->timestamp().value());
    ClockService::clock_type recvTime (p.annotation<annotations::Timestamp>().as_clock_type());
    ClockService::clock_type delay (recvTime - sendTime);
    if (nPackets_ == 0 || delay < minDelayStat_)
        minDelayStat_ = delay;
    if (delay > maxDelayStat_)
        maxDelayStat_ = delay;
    totalDelayStat_ += delay;
    ++nPackets_;
    if (delay > maxPacketDelay_)
        maxPacketDelay_ = delay;
    if (delay < ClockService::clock_type(0)) {
        SENF_LOG((senf::log::IMPORTANT)
                 ("Detected time reversal (\?\?) in packet from nodeId,ifIndex " <<
                  p->nodeId() << "," << p->interfaceIndex()));
        SENF_LOG((senf::log::NOTICE)(dumpPacketInfo(p)));
    }
    else if (delay > maxDelay_) {
        SENF_LOG((senf::log::IMPORTANT)
                 ("Detected excessive packet delay of "
                  << ClockService::reltime(delay)
                  << " in packet from nodeId,ifIndex "
                  << p->nodeId() << "," << p->interfaceIndex()));
        SENF_LOG((senf::log::NOTICE)(dumpPacketInfo(p)));
        ++ delayedPackets_;
    }
}

prefix_ void senf::emu::detail::DelayTimeChecker::statisticsUpdate()
{
    if (nPackets_ == 0)
        nPackets_ = 1;
    signals.packetDelay(
            double(ClockService::in_nanoseconds(minDelayStat_)) /
                ClockService::in_nanoseconds(ClockService::seconds(1)),
            double(ClockService::in_nanoseconds(totalDelayStat_)) /
                nPackets_ / ClockService::in_nanoseconds(ClockService::seconds(1)),
            double(ClockService::in_nanoseconds(maxDelayStat_)) /
                ClockService::in_nanoseconds(ClockService::seconds(1)));
    minDelayStat_ = totalDelayStat_ = maxDelayStat_ = ClockService::clock_type(0);
    nPackets_ = 0u;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::EmulatedInterface

prefix_ void senf::emu::EmulatedInterface::init()
{
    namespace fty = console::factory;

    interface().consoleDir()
        .add("emulationAddress",
             fty::Command(SENF_MEMBINDFNP(UDPClientHandle::Address, EmulatedInterface,
                                          emulationAddress, () const))
             .doc("The emulation address is the multicast address and port to which all emulated\n"
                  "Network traffic is sent. All emulated interfaces with the same emulation address\n"
                  "Are connected to the same network segment.")
             .overloadDoc("Get current emulation address") );
    interface().consoleDir()
        .add("emulationInterface",
             fty::Command(SENF_MEMBINDFNP(void, EmulatedInterface,
                                          emulationInterface, (std::string const &)))
             .arg("iface","name of interface for emulation network traffic")
             .doc("All emulation traffic is sent and received on this interface.")
             .overloadDoc("Set the emulation interface.") );
    interface().consoleDir()
        .add("emulationInterface",
             fty::Command(SENF_MEMBINDFNP(std::string const &, EmulatedInterface,
                                          emulationInterface, () const))
             .overloadDoc("Get current emulation interface.") );
}

prefix_ void senf::emu::EmulatedInterface::enableEmulatedInterface()
{
    if (address_ && ! enabled_)
        openSocket();
    enabled_ = true;
}

prefix_ void senf::emu::EmulatedInterface::disableEmulatedInterface()
{
    if (address_ && enabled_)
        closeSocket();
    enabled_ = false;
}

prefix_ void
senf::emu::EmulatedInterface::emulationAddress(UDPClientHandle::Address const & address)
{
    if (address_ && enabled_)
        closeSocket();
    address_ = address;
    if (address_ && enabled_)
        openSocket();
}

prefix_ void senf::emu::EmulatedInterface::openSocket()
{
    socket_ = UDPClientHandle();
    socket_.blocking(false);
    socket_.protocol().sndbuf(  48*1024);
    socket_.protocol().rcvbuf( 128*1024);
    v_initEmulationInterface();
}

prefix_ void senf::emu::EmulatedInterface::closeSocket()
{
    v_deinitEmulationInterface();
    socket_.close();
    socket_ = UDPClientHandle(senf::noinit);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::EmulatedReceiver

prefix_ senf::emu::EmulatedReceiver::EmulatedReceiver()
    : filter_ (*this), dropper_(0u), receiverJack (advance_.output), promisc_ (false), annotationMode_(false)
{
    ppi::connect(source_, filter_);
    ppi::connect(filter_, dropChecker_);
    ppi::connect(dropChecker_, delayChecker_);
    ppi::connect(delayChecker_, dropper_);
    ppi::connect(dropper_, advance_);

    source_.maxBurst(8);
}

prefix_ void senf::emu::EmulatedReceiver::init()
{
    namespace kw = console::kw;
    namespace fty = console::factory;

    basei::interface()
        .startStatistics.connect(membind(&EmulatedReceiver::startStatistics, this));
    basei::interface()
        .registerStatistics("in-droppedPacketsPerSecond", dropChecker_.signals.droppedPacketsPerSecond);
    basei::interface()
        .registerStatistics("in-packetDelay", delayChecker_.signals.packetDelay);
    basei::interface()
        .registerStatistics("in-numPeers", dropChecker_.signals.numPeers);

    basei::interface().consoleDir()
        .add("maxDelay",
             fty::Command(SENF_MEMBINDFNP(void, EmulatedReceiver,
                                          maxDelay, (ClockService::clock_type)))
             .arg("delay", "Maximum allowed packet delay",
                  kw::parser=senf::parseClockServiceInterval)
             .doc("Get/set maximum allowed packet delay.\n"
                  "Packets which take longer to be delivered will be logged.") );
    basei::interface().consoleDir()
        .add("maxDelay",
             fty::Command(SENF_MEMBINDFNP(ClockService::clock_type, EmulatedReceiver,
                                          maxDelay, () const))
             .formatter(senf::formatClockServiceInterval) );
    basei::interface().consoleDir()
        .add("delayedPackets", fty::Command(&EmulatedReceiver::delayedPackets, this)
             .doc("Number of packets with delay > maxDelay.") );
    basei::interface().consoleDir()
        .add("maxPacketDelay", fty::Command(&EmulatedReceiver::maxPacketDelay, this)
             .doc("Maximum packet delay encountered.") );
    basei::interface().consoleDir()
        .add("droppedPackets", fty::Command(&EmulatedReceiver::droppedPackets, this)
             .doc("Number of dropped packets.") );

    basei::interface().consoleDir()
        .add("lossrate",
             fty::Command(SENF_MEMBINDFNP(void, EmulatedReceiver,
                                          lossRate, (double)))
             .arg("lossRate", "loss rate of packet dropper")
             .doc("Get/set the loss rate of the packet dropper.") );
    basei::interface().consoleDir()
        .add("lossrate",
             fty::Command(SENF_MEMBINDFNP(double, EmulatedReceiver,
                                          lossRate, () const)) );
}

prefix_ void senf::emu::EmulatedReceiver::initEmulatedReceiver()
{
    EmulatedInterface & iface (emui::interface());
    iface.emulationSocket().protocol().reuseaddr(true);
    iface.emulationSocket().bind(iface.emulationAddress());
    if (!iface.emulationInterface().empty()) {
        iface.emulationSocket().protocol().bindInterface(iface.emulationInterface());
        iface.emulationSocket().protocol().mcAddMembership(iface.emulationAddress().address(),
                                                           iface.emulationInterface());
    } else
        iface.emulationSocket().protocol().mcAddMembership(iface.emulationAddress().address());
    iface.emulationSocket().protocol().mcLoop(true);
    source_.handle(iface.emulationSocket());
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::EmulatedTransmitter

prefix_ senf::emu::EmulatedTransmitter::EmulatedTransmitter()
    : transmitterJack(delayer_.input), filter_(*this)
{
    ppi::connect(delayer_, filter_);
    ppi::connect(filter_, sink_);
}

prefix_ void senf::emu::EmulatedTransmitter::init()
{
    namespace fty = console::factory;

    basei::interface().consoleDir()
        .add("delay",
             fty::Command(SENF_MEMBINDFNP(void, EmulatedTransmitter,
                                          delay, (ClockService::clock_type)))
             .arg("delay", "transmitting delay",
                  console::kw::parser=senf::parseClockServiceInterval)
             .doc("TODO.\n"
                  ".") ); //TODO: delay doc for console
    basei::interface().consoleDir()
        .add("delay",
             fty::Command(SENF_MEMBINDFNP(ClockService::clock_type, EmulatedTransmitter,
                                          delay, () const))
             .formatter(senf::formatClockServiceInterval) );
}

prefix_ void senf::emu::EmulatedTransmitter::initEmulatedTransmitter()
{
    EmulatedInterface & iface (emui::interface());
    if (!iface.emulationInterface().empty())
        iface.emulationSocket().protocol().bindInterface(iface.emulationInterface());
    sink_.handle(iface.emulationSocket());
    sink_.writer().target(emui::interface().emulationAddress());
}

prefix_ senf::Packet::size_type senf::emu::EmulatedTransmitter::v_emulatedPayloadSize(Packet packet)
{
    EthernetPacket ethp (packet.find<EthernetPacket>(nothrow));
    if (!ethp)
        return 0;
    Packet pp (ethp.next(nothrow));
    return pp ? pp.size() : 0u;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::EmulatedWiredInterface

prefix_ void senf::emu::EmulatedWiredInterface::init()
{
    namespace fty = console::factory;

    EmulatedInterface::init();
    interface().consoleDir()
        .add("emulationAddress",
             fty::Command(SENF_MEMBINDFNP(void, EmulatedWiredInterface,
                                          emulationAddress, (UDPClientHandle::Address const &)))
             .arg("address", "emulation multicast address")
             .overloadDoc("Change emulation address.") );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::EmulatedWiredReceiver

prefix_ void senf::emu::EmulatedWiredReceiver::init()
{
    EmulatedReceiver::init();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::EmulatedWiredTransmitter

prefix_ void senf::emu::EmulatedWiredTransmitter::init()
{
    EmulatedTransmitter::init();
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
