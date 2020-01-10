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
    \brief EmulatedWirelessInterface non-inline non-template implementation */

#include "EmulatedWirelessInterface.hh"

// Custom includes
#include <unistd.h>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <senf/Utils/Console/Sysdir.hh>
#include <senf/Utils/Console/Variables.hh>
#include <senf/Utils/Console/STLSupport.hh>
#include <senf/Utils/algorithm.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::EmulatedWirelessInterfaceTransmitFilter

prefix_ void senf::emu::detail::EmulatedWirelessInterfaceTransmitFilter::request()
{
    Packet p (input());
    if (iface_.basei::interface().modulationParameterIds().empty())
        return;
    EmulatedWirelessPacketHeader h (EmulatedWirelessPacketHeader::createBefore(p));
    h->modulationId() = iface_.basei::interface().transmitter().modulation();
    h->txPower() = iface_.basei::interface().transmitter().txPower() / 100;
    h->frequency() = iface_.basei::interface().frequency();
    h->bandwidth() = iface_.basei::interface().bandwidth();
    h.finalizeThis();
    output(h);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::EmulatedWirelessInterfaceReceiveFilter

prefix_ void senf::emu::detail::EmulatedWirelessInterfaceReceiveFilter::request()
{
    EmulatedWirelessPacketHeader p (input());

    // filter packets which are sent before the channel is changed
    if (iface_.basei::interface().frequency() != p->frequency() ||
        iface_.basei::interface().bandwidth() != p->bandwidth()) return;

    Packet n (p.next(senf::nothrow));
    if (!n)
        return;
    if (! senf::contains(iface_.basei::interface().modulationParameterIds(), p->modulationId())) {
        SENF_LOG((senf::log::IMPORTANT)("Dropping packet with unknown wireless modulation parameter " << p->modulationId()));
        return;
    }
    p.annotation<annotations::WirelessModulation>().id = p->modulationId();
    if (! iface_.v_emulatedWirelessQuality(n, p.annotation<annotations::Quality>()))
        return;

    output(n);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::EmulatedFrequencyRegistry

prefix_ void senf::emu::EmulatedFrequencyRegistry::allocate(unsigned interfaceId,
                                                            unsigned frequency,
                                                            unsigned bandwidth, UserCallback cb)
{
    namespace l = boost::lambda;
    using boost::lambda::_1;

    checkCollisionOk(frequency, bandwidth);

    Entries::index_iterator<ByFrequency>::type i (
        entries_.get<ByFrequency>().find(boost::make_tuple(frequency,bandwidth)));
    if (i == entries_.end()) {
        Entry e (frequency, bandwidth);
        if (running_) {
            e.address = getAddress();
            e.ownerNodeId = Interface::nodeId();
            sendAdd(Interface::nodeId(), frequency, bandwidth, e.address);
        }
        i = entries_.insert(e).first;
    }
    users_.insert(User(i, interfaceId, cb));
    if (running_) {
        sendJoin(Interface::nodeId(), frequency, bandwidth);
        entries_.modify(i, (++ l::bind(&Entry::nUsers, _1)));
        channelPopulationChanged(*i,Interface::nodeId(),JOIN);
    }
    cb(i->address);
    if (! server_ && group_)
        schedulerStart_.enable();
}

prefix_ void senf::emu::EmulatedFrequencyRegistry::release(unsigned interfaceId)
{
    namespace l = boost::lambda;
    using boost::lambda::_1;

    Users::index_iterator<ByInterfaceId>::type i (users_.get<ByInterfaceId>().find(interfaceId));
    SENF_ASSERT( i != users_.end(),
                 "senf::emu::EmulatedFrequencyRegistry::release(): "
                 "No registration for interface" );
    sendLeave(Interface::nodeId(), i->entry->frequency, i->entry->bandwidth);
    if (i->entry->nUsers > 0 && running_) {
        entries_.modify(i->entry, -- l::bind(&Entry::nUsers, _1));
        channelPopulationChanged(*i->entry,Interface::nodeId(),LEAVE);
    } // Otherwise there was a stop/release race
    if (i->entry->nUsers == 0 && i->entry->ownerNodeId == Interface::nodeId()) {
        sendDel(Interface::nodeId(), i->entry->frequency, i->entry->bandwidth);
        entries_.get<ByFrequency>().erase(i->entry);
    }
    users_.erase(i);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// private members

prefix_ senf::emu::EmulatedFrequencyRegistry::EmulatedFrequencyRegistry()
    : socket_ (), group_ ("239.202.104.1:4701"), server_ (),
      schedulerStart_ ("senf::emu::EmulatedFrequencyRegistry::schedulerStart",
                       membind(&EmulatedFrequencyRegistry::schedulerStart, this), false),
      channelGroupRange_ ("239.202.108.0/22"), portbase_ (11264u), lastAddress_ (perturb(Interface::nodeId())),
      entries_ (), users_ (), running_ (false)
{
    namespace fty = console::factory;

    console::sysdir().add("emu", dir_);

    dir_.add("consoleGroup",
             fty::Command(SENF_MEMBINDFNP(UDPClientHandle::Address, EmulatedFrequencyRegistry,
                                          consoleGroup, () const)));
    dir_.add("consoleGroup",
             fty::Command(SENF_MEMBINDFNP(void, EmulatedFrequencyRegistry,
                                          consoleGroup, (UDPClientHandle::Address const &))));

    typedef std::pair<INet4Network, unsigned> AddressPair;
    dir_.add("addressRange",
             fty::Command(SENF_MEMBINDFNP(AddressPair, EmulatedFrequencyRegistry,
                                          addressRange, () const)));
    dir_.add("addressRange",
             fty::Command(SENF_MEMBINDFNP(void, EmulatedFrequencyRegistry,
                                          addressRange, (INet4Network const &, unsigned))));

    dir_.add("list", fty::Command(&EmulatedFrequencyRegistry::consoleList, this));
    dir_.add("nodeId", fty::Command( SENF_FNP( unsigned, Interface::nodeId, ())) );

    console::DirectoryNode & ipcdir (dir_.add("ipc",fty::Directory()));
    ipcdir.add("add", fty::Command(&EmulatedFrequencyRegistry::add, this));
    ipcdir.add("del", fty::Command(&EmulatedFrequencyRegistry::del, this));
    ipcdir.add("join", fty::Command(&EmulatedFrequencyRegistry::join, this));
    ipcdir.add("leave", fty::Command(&EmulatedFrequencyRegistry::leave, this));
    ipcdir.add("poll", fty::Command(&EmulatedFrequencyRegistry::poll, this));
    ipcdir.add("start", fty::Command(&EmulatedFrequencyRegistry::start, this));
    ipcdir.add("stop", fty::Command(&EmulatedFrequencyRegistry::stop, this));
}

prefix_ void senf::emu::EmulatedFrequencyRegistry::schedulerStart()
{
    schedulerStart_.disable();
    if (group_) {
        server_.reset(new console::UDPServer(group_));
        server_->root(dir_["ipc"]).emptyReplies(false);
    }
}

prefix_ void senf::emu::EmulatedFrequencyRegistry::consoleList(std::ostream & os)
    const
{
    boost::format fmt ("%10d %10d  %-21s%5d %10d ");
    os << fmt % "FREQUENCY" % "BANDWIDTH" % "ADDRESS" % "NODES" % "OWNER" << " USERS\n";
    EntriesRange e (entries());
    for (EntriesRange::iterator i (e.begin()); i != e.end(); ++i) {
        os << fmt
            % i->frequency
            % i->bandwidth
            % i->address
            % i->nUsers
            % i->ownerNodeId;
        UsersRange u (users(i));
        for (UsersRange::iterator j (u.begin()); j != u.end(); ++j)
            os << " " << *j;
        os << '\n';
    }
    os << (running_ ? " running\n" : " halted\n");
}

prefix_ void senf::emu::EmulatedFrequencyRegistry::add(unsigned nodeId, unsigned frequency,
                                                       unsigned bandwidth,
                                                       UDPClientHandle::Address address)
{
    namespace l = boost::lambda;
    using boost::lambda::_1;

    if (nodeId == Interface::nodeId())
        // drop looped commands
        return;
    Entries::index_iterator<ByFrequency>::type fi (
        entries_.get<ByFrequency>().find(boost::make_tuple(frequency,bandwidth)));
    Entries::index_iterator<ByAddress>::type ai (entries_.get<ByAddress>().find(address));

    if (fi != entries_.get<ByFrequency>().end()
        && ai != entries_.get<ByAddress>().end()
        && &*fi == &*ai) {
        // special case: Exact same entry already exists. Just update owner if necessary
        if (nodeId < fi->ownerNodeId)
            entries_.modify(fi, l::bind(&Entry::ownerNodeId, _1) = nodeId);
        return;
    }

    if (ai != entries_.get<ByAddress>().end()) {
        // Same address is already in use by a different entry.
        // The lower nodeId will get the requested address.
        // If the loser is me (the losing entry has my own nodeId), select a new address and send
        // a new 'add' message
        if (nodeId < ai->ownerNodeId) {
            UDPClientHandle::Address newAddr;
            if (ai->ownerNodeId == Interface::nodeId()) {
                // getAddress() here will find an address *different* from address since that address
                // is still in the table ...
                newAddr = getAddress();
                sendAdd(Interface::nodeId(), ai->frequency, ai->bandwidth, newAddr);
            }
            entries_.get<ByAddress>().modify_key(ai, _1 = newAddr);
            updateAddress(*ai);
        }
        else
            return;
    }

    if (fi != entries_.get<ByFrequency>().end()) {
        // Entry exists for this frequency but possibly with different address.
        // We use the address of the lower nodeId.
        if (! fi->address || (fi->address != address && nodeId <= fi->ownerNodeId)) {
            entries_.get<ByFrequency>().modify(
                fi, (l::bind(&Entry::address,_1) = address,
                     l::bind(&Entry::ownerNodeId,_1) = nodeId));
            updateAddress(*fi);
        }
    } else
        entries_.insert(Entry(frequency, bandwidth, address, 0, nodeId));
}

prefix_ void senf::emu::EmulatedFrequencyRegistry::del(unsigned nodeId, unsigned frequency,
                                                       unsigned bandwidth)
{
    namespace l = boost::lambda;
    using boost::lambda::_1;

    if (nodeId == Interface::nodeId())
        // drop looped commands
        return;
    Entries::index_iterator<ByFrequency>::type i (
        entries_.get<ByFrequency>().find(boost::make_tuple(frequency,bandwidth)));
    if (i == entries_.get<ByFrequency>().end())
        return;
    // Only the owner may remove an entry
    if (nodeId != i->ownerNodeId)
        return;
    if (users_.get<ByEntry>().count(i) > 0) {
        SENF_LOG((senf::log::CRITICAL)(
                     "Active entry " << frequency << "," << bandwidth <<
                     " removed by " << nodeId));
        entries_.get<ByFrequency>().modify(
            i, (l::bind(&Entry::address,_1) = UDPClientHandle::Address(),
                l::bind(&Entry::ownerNodeId,_1) = 0));
        updateAddress(*i);
    }
    else
        entries_.get<ByFrequency>().erase(i);
}

prefix_ void senf::emu::EmulatedFrequencyRegistry::join(unsigned nodeId, unsigned frequency,
                                                        unsigned bandwidth)
{
    namespace l = boost::lambda;
    using boost::lambda::_1;

    if (nodeId == Interface::nodeId())
        // drop looped commands
        return;
    Entries::index_iterator<ByFrequency>::type i (
        entries_.get<ByFrequency>().find(boost::make_tuple(frequency,bandwidth)));
    if (i == entries_.end()) {
        // Probably out-of-order packets: Create an entry with empty address. This will
        // later be filled with a valid address
        i = entries_.insert(Entry(frequency, bandwidth, UDPClientHandle::Address(), 1, 0)).first;
        return;
    } else
        entries_.modify(i, ++ l::bind(&Entry::nUsers, _1));
    channelPopulationChanged(*i, nodeId, JOIN);
}

prefix_ void senf::emu::EmulatedFrequencyRegistry::leave(unsigned nodeId, unsigned frequency,
                                                         unsigned bandwidth)
{
    namespace l = boost::lambda;
    using boost::lambda::_1;

    if (nodeId == Interface::nodeId())
        // drop looped commands
        return;
    Entries::index_iterator<ByFrequency>::type i (
        entries_.get<ByFrequency>().find(boost::make_tuple(frequency,bandwidth)));
    if (i == entries_.end())
        return;
    if (i->nUsers > 0) {
        entries_.modify(i, -- l::bind(&Entry::nUsers, _1));
        channelPopulationChanged(*i, nodeId, LEAVE);
    } // Otherwise there was a stop/leave race
    if (i->nUsers == 0 && (i->ownerNodeId == Interface::nodeId() || i->ownerNodeId == 0)) {
        if (users_.get<ByEntry>().count(i) > 0) {
            SENF_LOG((senf::log::CRITICAL)(
                         "Disabling active entry " << frequency << "," << bandwidth <<
                         ": no users?"));
            if (i->address) {
                entries_.get<ByFrequency>().modify(
                    i, (l::bind(&Entry::address,_1) = UDPClientHandle::Address(),
                        l::bind(&Entry::ownerNodeId,_1) = 0));
                updateAddress(*i);
            }
        }
        else {
            if (i->ownerNodeId == Interface::nodeId())
                sendDel( Interface::nodeId(), i->frequency, i->bandwidth);
            entries_.get<ByFrequency>().erase(i);
        }
    }
}

prefix_ int senf::emu::EmulatedFrequencyRegistry::poll(unsigned nodeId, unsigned frequency,
                                                       unsigned bandwidth)
{
    if (nodeId == Interface::nodeId())
        // drop looped commands
        return 0;
    Entries::index_iterator<ByFrequency>::type i (
        entries_.get<ByFrequency>().find(boost::make_tuple(frequency,bandwidth)));
    if (i == entries_.end())
        return 0;
    std::pair<Users::index_iterator<ByEntry>::type,Users::index_iterator<ByEntry>::type> e (
        users_.get<ByEntry>().equal_range(i));
    return std::distance(e.first, e.second);
}

prefix_ void senf::emu::EmulatedFrequencyRegistry::start()
{
    namespace l = boost::lambda;
    using boost::lambda::_1;

    if (! server_ && group_)
        schedulerStart();
    if (running_)
        return;
    SENF_LOG((senf::log::NOTICE)("Starting frequency allocation"));
    for (Entries::iterator i (entries_.begin()), i_end (entries_.end()); i != i_end; ++i) {
        unsigned users (users_.get<ByEntry>().count(i));
        if (users > 0) {
            if (! i->ownerNodeId && ! i->address) {
                entries_.modify(i, (l::bind(&Entry::address,_1) = getAddress(),
                                    l::bind(&Entry::ownerNodeId,_1) = Interface::nodeId()));
                updateAddress(*i);
                sendAdd(Interface::nodeId(), i->frequency, i->bandwidth, i->address);
            }
            for (; users > 0; --users) {
                sendJoin(Interface::nodeId(), i->frequency, i->bandwidth);
                entries_.modify(i, ++ l::bind(&Entry::nUsers, _1));
                channelPopulationChanged(*i, Interface::nodeId(), JOIN);
            }
        }
    }
    running_ = true;
}

prefix_ void senf::emu::EmulatedFrequencyRegistry::stop()
{
    namespace l = boost::lambda;
    using boost::lambda::_1;

    if (! running_)
        return;
    SENF_LOG((senf::log::NOTICE)("Stopping frequency allocation"));
    if (entries_.empty()) {
        running_ = false;
        return;
    }
    Entries::iterator i (entries_.begin());
    Entries::iterator i_next (boost::next(i));
    Entries::iterator const i_end (entries_.end());
    for (;i != i_end; i = i_next, ++ i_next) {
        entries_.modify(i, (l::bind(&Entry::address,_1) = UDPClientHandle::Address(),
                            l::bind(&Entry::ownerNodeId,_1) = 0,
                            l::bind(&Entry::nUsers,_1) = 0));
        channelPopulationChanged(*i, Interface::nodeId(), STOP);
        if (users_.get<ByEntry>().count(i) == 0)
            entries_.erase(i);
        else
            updateAddress(*i);
    }
    running_ = false;
}

prefix_ void senf::emu::EmulatedFrequencyRegistry::sendAdd(unsigned nodeId, unsigned frequency,
                                                           unsigned bandwidth,
                                                           UDPClientHandle::Address address)
{
    socket_.writeto(group_, (boost::format("add %d %d %d %s")
                             % nodeId % frequency % bandwidth % address).str());
}

prefix_ void senf::emu::EmulatedFrequencyRegistry::sendDel(unsigned nodeId, unsigned frequency,
                                                           unsigned bandwidth)
{
    socket_.writeto(group_, (boost::format("del %d %d %d")
                             % nodeId % frequency % bandwidth).str());
}

prefix_ void senf::emu::EmulatedFrequencyRegistry::sendJoin(unsigned nodeId,
                                                            unsigned frequency,
                                                            unsigned bandwidth)
{
    socket_.writeto(group_, (boost::format("join %d %d %d")
                             % nodeId % frequency % bandwidth).str());
}

prefix_ void senf::emu::EmulatedFrequencyRegistry::sendLeave(unsigned nodeId,
                                                             unsigned frequency,
                                                             unsigned bandwidth)
{
    socket_.writeto(group_, (boost::format("leave %d %d %d")
                             % nodeId % frequency % bandwidth).str());
}

prefix_ void senf::emu::EmulatedFrequencyRegistry::sendPoll(unsigned nodeId,
                                                            unsigned frequency,
                                                            unsigned bandwidth)
{
    socket_.writeto(group_, (boost::format("poll %d %d %d")
                             % nodeId % frequency % bandwidth).str());
}

prefix_ senf::emu::UDPClientHandle::Address senf::emu::EmulatedFrequencyRegistry::getAddress()
{
    for (;;) {
        lastAddress_ = (lastAddress_+1) & ((1u<<(32-channelGroupRange_.prefix_len()))-1);
        UDPClientHandle::Address addr (channelGroupRange_.host(lastAddress_),
                                       portbase_ + lastAddress_);
        Entries::index_iterator<ByAddress>::type i (
            entries_.get<ByAddress>().find(addr));
        if (i == entries_.get<ByAddress>().end())
            return addr;
    }
    return UDPClientHandle::Address();
}

prefix_ void senf::emu::EmulatedFrequencyRegistry::updateAddress(Entry const & entry)
{
    Users::index_iterator<ByEntry>::type i, i_end;
    boost::tie(i, i_end) = users_.get<ByEntry>().equal_range(&entry);
    for (; i != i_end; ++i)
        i->callback(entry.address);
}

namespace {

    inline bool frequencyOverlap(unsigned f1, unsigned  b1, unsigned  f2, unsigned b2)
    {
        return 2*f1-b1 <= 2*f2+b2 && 2*f2-b2 <= 2*f1+b1;
    }

}

prefix_ void senf::emu::EmulatedFrequencyRegistry::checkCollisionOk(unsigned frequency,
                                                                    unsigned bandwidth)
{
    if (! cb_)
        return;
    for (Entries::index_iterator<ByFrequency>::type i (entries_.get<ByFrequency>().begin()),
             i_end (entries_.get<ByFrequency>().end()); i != i_end; ++i)
        if (frequencyOverlap(frequency, bandwidth, i->frequency, i->bandwidth)) {
            cb_(frequency, bandwidth);
            return;
        }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::EmulatedWirelessInterface

prefix_ senf::emu::EmulatedWirelessInterface::~EmulatedWirelessInterface()
{
    // Need to use cached index_ value since interface() is not accessible from
    // the destructor
    if (index_)
        emulatedFrequencyRegistry().release(index_);
}

prefix_ void senf::emu::EmulatedWirelessInterface::registerFrequency(Ranges const & frequencies,
                                                                     Ranges const & bandwidths)
{
    for (Ranges::const_iterator i (frequencies.begin()), i_end (frequencies.end()); i != i_end; ++i)
        for (Ranges::const_iterator j (bandwidths.begin()), j_end (bandwidths.end()); j != j_end; ++j)
            registerFrequency(i->lower, i->upper, j->lower, j->upper);
}

prefix_ void senf::emu::EmulatedWirelessInterface::init()
{
    EmulatedInterface::init();

    namespace fty = console::factory;
    interface().consoleDir()
        .add("registerFrequency",
             fty::Command(SENF_MEMBINDFNP(void, EmulatedWirelessInterface, registerFrequency,
                                          (Ranges const &, Ranges const &)))
             .arg("frequencies","channel frequencies in kHz")
             .arg("bandwidths", "channel bandwidths in kHz")
             .doc("Register valid frequency/bandwidth combinations.\n"
                  "Both parameters /frequencies/ and /bandwidths/ may be either a single value or\n"
                  "range or a list of values or ranges.\n"
                  "\n"
                  "The statement will register all combinations of any frequency with any bandwidth.\n"
                  "\n"
                  "Examples:\n"
                  "    registerFrequency 2412000 20000;\n"
                  "        Register a single frequency/bandwidth combination.\n"
                  "\n"
                  "    registerFrequency 1000000:2000000 5000:20000;\n"
                  "        Register a single frequency/bandwidth range combination.\n"
                  "\n"
                  "    registerFrequency (2412000 2417000 2422000) 20000;\n"
                  "        Register 3 frequency/bandwidth combinations;\n"
                  "\n"
                  "    registerFrequency (1000000:2000000 3000000:4000000) (5000 10000:12000 20000);"
                  "        Register 6 frequency/bandwidth combinations.") );
}

prefix_ void senf::emu::EmulatedWirelessInterface::emulatedFrequency(unsigned frequency,
                                                                     unsigned bandwidth)
{
    frequency_ = frequency;
    bandwidth_ = bandwidth;
    // cache index value for destructor
    index_ = interface().index();
    emulatedFrequencyRegistry().allocate(
        index_, frequency, bandwidth,
        SENF_MEMBINDFNP(void, EmulatedWirelessInterface,
                        emulationAddress, (UDPClientHandle::Address const &)));
}

prefix_ void senf::emu::EmulatedWirelessInterface::emulatedCoverageRange(unsigned distance)
{
    // not supported.
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::EmulatedWirelessReceiver

prefix_ void senf::emu::EmulatedWirelessReceiver::init()
{
    EmulatedReceiver::init();

    namespace fty = console::factory;

    (*this).basei::interface().consoleDir()
        .add("rssiRandomnessRange",
                fty::Variable(rssiRandomnessRange_)
        .doc("Set the range of the random variation of the RSSI value") );
    (*this).basei::interface().consoleDir()
        .add("noiseRandomnessRange",
                fty::Variable(noiseRandomnessRange_)
        .doc("Set the range of the random variation of the Noise value") );
}


prefix_ bool
senf::emu::EmulatedWirelessReceiver::v_emulatedWirelessQuality(Packet packet,
                                                               annotations::Quality & quality)
{
    //we need the distance between two nodes otherwise we can't calculate the attenuation
    if (!emui::interface().distanceCallback_) {
        SENF_LOG( (senf::log::VERBOSE) ("no distance callback function given") );
        return true;
    }

    PacketHeader ph = packet.rfind<PacketHeader>();
    EmulatedWirelessPacketHeader ewph = packet.rfind<EmulatedWirelessPacketHeader>();

    double d = emui::interface().distanceCallback_(ph->nodeId());

    //distances equal to or smaller than 0 are invalid
    if (d <=0 )
        return true;

    short minRssi = ModulationParameterRegistry::instance().findModulationById(
            packet.annotation<annotations::WirelessModulation>().id).minRssi;
    unsigned f = (*this).basei::interface().frequency();

    //free space loss
    // d in km
    // f in khz
    // 3.0e2 equals to c [1e3*1e3/c=3.0e2]
    double fsl = 20 * log10( 4*d*M_PI*f/3.0e2);

    short rssi = short(ewph->txPower()-fsl);

    if (rssiRandomnessRange_ != 0)
        rssi += rand() % rssiRandomnessRange_; // add some randomness

    if (rssi <= minRssi) {
        SENF_LOG( (senf::log::VERBOSE) ("FSL to high. Packet is dropped"));
        SENF_LOG( (senf::log::VERBOSE) (
                   "minRSSI " << minRssi  << "\n"
                << "txpower: " << ewph->txPower() << "\n"
                << "frequency (in KHz): " << f<< "\n"
                << "transmitter node:" << ph->nodeId() << "\n"
                << "distance " << emui::interface().distanceCallback_(ph->nodeId()) << " km\n"
                << "Free Space Loss: " << fsl << " dB\n"
                << "RSSI: " << rssi << " dB\n") );
        return false;
    }

    int defaultNoiseValue = -110;
    if (noiseRandomnessRange_ !=0)
        defaultNoiseValue=defaultNoiseValue + (rand()%noiseRandomnessRange_);

    //annotate packet with calculated values
    quality.rssi = rssi;
    quality.noise = defaultNoiseValue;
    quality.snr = quality.rssi - quality.noise;

    //set fake flags. we need a better algo here
    quality.flags.frameCorrupt = (rand() % ((rssi-minRssi)*(rssi-minRssi))) > 1 ? false : true;
    quality.flags.frameRetransmitted = (rand() % ((rssi-minRssi)*(rssi-minRssi))) > 2 ? false : true;
    quality.flags.frameLength = ewph.next().size();
    return true;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::EmulatedWirelessTransmitter

prefix_ void senf::emu::EmulatedWirelessTransmitter::registerTxPower(Ranges const & powers)
{
    for (Ranges::const_iterator i (powers.begin()), i_end (powers.end()); i != i_end; ++i)
        registerTxPower(i->lower, i->upper);
}

prefix_ void senf::emu::EmulatedWirelessTransmitter::init()
{
    EmulatedTransmitter::init();

    namespace fty = console::factory;

    basei::interface().consoleDir()
        .add("registerTxPower",
             fty::Command(SENF_MEMBINDFNP(void, EmulatedWirelessTransmitter,
                                          registerTxPower, (Ranges const &)))
             .arg("powers", "transmit power values or ranges")
             .doc("Register valid txpower values or ranges.\n"
                  "The /powers/ parameter may be either a single value or range or a list of values\n"
                  "or ranges:\n\n"
                  "    registerTxPower 4;\n"
                  "        Register single txpower value\n\n"
                  "    registerTxPower (-4:-1 1:10);\n"
                  "        Register two txpower ranges\n") );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "EmulatedWirelessInterface.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
