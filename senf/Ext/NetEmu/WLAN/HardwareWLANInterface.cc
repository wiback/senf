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
   // @tho: There must be a better way to decide which mcs/legacy modualtion to provide for either 2.4 or 5.x GHz bands

/** \file
    \brief HardwareWLANInterface non-inline non-template implementation */

#include "HardwareWLANInterface.hh"

// Custom includes
#include <linux/filter.h>
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/icl/interval_set.hpp>
#include <senf/Utils/algorithm.hh>
#include <senf/Ext/NetEmu/Annotations.hh>
#include <senf/Ext/NetEmu/config.hh>
#include <senf/Ext/NetEmu/Log.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

#define MHZ_TO_KHZ(freq) ((freq) * 1000)

#define IGNORE_EXCPETION(expr)      \
    try {                           \
        expr                        \
    } catch (std::exception &) {};

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::HardwareWLANInterfaceNet

prefix_ senf::emu::detail::HardwareWLANInterfaceNet::HardwareWLANInterfaceNet()
    : socket (senf::noinit),
      source (socket),
      sink (socket),
      monSocket (senf::noinit),
      monSource(monSocket),
      netOutput (monitorDataFilter.output), netInput (sink.input)
{
    ppi::connect(monSource, monitorDataFilter);
    ppi::connect(source, monitorDataFilter.input_plain);
}

prefix_ void senf::emu::detail::HardwareWLANInterfaceNet::assignMonitorSocket(ConnectedMMapReadPacketSocketHandle & monSocket_)
{
    monSocket = monSocket_;
    monSource.handle(monSocket);
}

prefix_ void senf::emu::detail::HardwareWLANInterfaceNet::assignDataSocket(ConnectedMMapPacketSocketHandle & socket_)
{
    socket = socket_;
    source.handle(socket);
    sink.handle(socket);
}

prefix_ void senf::emu::detail::HardwareWLANInterfaceNet::dataSource(bool on)
{    
    if (on) {
        source.handle(socket);
        // clean update the source queue, as we have changed the socket
        source.flush();
        // flush any pending packets from the filter
        monitorDataFilter.flushQueues();
	// flush old rxDropped values
        source.dropped();
    } else {
        source.handle(ConnectedMMapPacketSocketHandle(senf::noinit));
    }
}

prefix_ unsigned senf::emu::detail::HardwareWLANInterfaceNet::rcvBuf()
{
    if (socket)
        return socket.protocol().rcvbuf();
    return 0;
}

prefix_ void senf::emu::detail::HardwareWLANInterfaceNet::rcvBuf(unsigned rcvbuf)
{
    if (socket)
        socket.protocol().rcvbuf(rcvbuf);
}

prefix_ unsigned senf::emu::detail::HardwareWLANInterfaceNet::sndBuf()
{
    if (socket)
        return socket.protocol().sndbuf();
    return 0;
}

prefix_ void senf::emu::detail::HardwareWLANInterfaceNet::sndBuf(unsigned sndbuf)
{
    if (socket)
        socket.protocol().sndbuf(sndbuf);
}

prefix_ void senf::emu::detail::HardwareWLANInterfaceNet::flush()
{
    if (socket)
        source.flush();
    if (monSocket)
        monSource.flush();
    monitorDataFilter.flushQueues();
}

prefix_ senf::emu::TSFTHistogram & senf::emu::detail::HardwareWLANInterfaceNet::tsftHistogram()
{
    return monitorDataFilter.tsftHistogram();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::HardwareWLANInterface

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

namespace senf { namespace emu {
    SENF_CONSOLE_REGISTER_ENUM_MEMBER( WirelessNLController, Band_t, (BAND_2GHZ)(BAND_5GHZ) );
    SENF_CONSOLE_REGISTER_ENUM_MEMBER( HardwareWLANInterface::HTMode, Enum, (Disabled)(Enabled)(HT20only)(HT40only) );
}}

prefix_ senf::emu::HardwareWLANInterface::HardwareWLANInterface(std::pair<std::string,std::string> interfaces)
    : WLANInterface(detail::HardwareWLANInterfaceNet::netOutput, detail::HardwareWLANInterfaceNet::netInput),
      netctl_(interfaces.first), wnlc_(interfaces.first), dev_(interfaces.first), monitorDev_ (interfaces.second), spectralScanner_(wnlc_.phyName()),
      wifiStatistics_(monitorDevice()), promisc_(false), frequencyOffset_(0), restrictedBand_(-1), htMode_(HTMode::Disabled),
      modId_( WLANModulationParameterRegistry::instance().parameterIdUnknown()), bw_(0), txPower_(0),
      rcvBufSize_ (4096), sndBufSize_ (96*1024), qlen_ (512)
{
    init();
}

prefix_ void senf::emu::HardwareWLANInterface::init()
{
    if (wnlc_.hasHTCapabilities()) {
        switch (wnlc_.htCapabilities().channelWidth) {
        case HTCapabilitiesInfo::HT20:
            htMode(HTMode::HT20only);
            break;
        case HTCapabilitiesInfo::HT20_40:
            htMode(HTMode::Enabled);
            break;
        default:
            // this forces the monitorDataFilter to disable packet reordering
            htMode(HTMode::Disabled);
            break;
        }
    } else {
        htMode(HTMode::Disabled);
    }

    registerFrequencies();
    registerModulations();
    registerTxPowers();

    WLANInterface::init();
    HardwareInterface::init();

    namespace fty = console::factory;
    namespace kw = senf::console::kw;
    consoleDir()
        .add("tsftHistogram", HardwareWLANInterfaceNet::monitorDataFilter.tsftHistogram().dir);
    consoleDir()
        .add("filterStats", fty::Command( &HardwareWLANInterface::dumpFilterStats, this)
        .doc("dumps and resets the WLAN Data Filter statistics"));
    consoleDir()
        .add("mmapStats", fty::Command( &HardwareWLANInterface::dumpMmapStats, this)
        .doc("dumps and resets the MMAP RX/TX statistics"));
    consoleDir()
        .add("frequencyOffset", fty::Command(
                SENF_MEMBINDFNP(void, HardwareWLANInterface, frequencyOffset, (int)))
        .doc("set the frequency offset"));
    consoleDir()
        .add("frequencyOffset", fty::Command(
                 SENF_MEMBINDFNP(int, HardwareWLANInterface, frequencyOffset, () const))
        .overloadDoc("get the frequency offset"));
    consoleDir()
        .add("restrictBand", fty::Command(
                SENF_MEMBINDFNP(void, HardwareWLANInterface, restrictBand, (WirelessNLController::Band_t)))
        .arg("band", "'BAND_2GHZ' or 'BAND_5GHZ'")
        .doc("restrict the registered frequencies to given band") );
    consoleDir()
        .add("htMode", fty::Command(
                SENF_MEMBINDFNP(void, HardwareWLANInterface, htMode, (HTMode::Enum)))
        .arg("mode", "HT mode")
        .doc("setting HT mode") );
    consoleDir()
        .add("htMode", fty::Command(
                SENF_MEMBINDFNP(HTMode::Enum, HardwareWLANInterface, htMode, () const))
        .overloadDoc("get HT mode setting") );
    consoleDir()
        .add("restrictBand", fty::Command(
                SENF_MEMBINDFNP(void, HardwareWLANInterface, restrictBand, ()))
        .overloadDoc("clear restriction") );
    consoleDir()
        .add("coverageClass", fty::Command(
                SENF_MEMFNP(unsigned, WirelessNLController, coverageClass, ()), &wnlc_)
        .doc("get the coverage class"));
    consoleDir()
        .add("coverageClass", fty::Command(
                SENF_MEMFNP(void, WirelessNLController, coverageClass, (unsigned)), &wnlc_)
        .overloadDoc("set the coverage class"));
    consoleDir()
        .add("device", fty::Command(
                SENF_MEMBINDFNP(std::string const &, HardwareWLANInterface, device, () const))
        .doc("Get WLAN network device name.") );
    consoleDir()
        .add("monitorDevice", fty::Command(
                SENF_MEMBINDFNP(std::string const &, HardwareWLANInterface, monitorDevice, () const))
        .overloadDoc("Get monitor device name.\n"
                     "If the device name is empty, data is read from the normal device"
                     "and no monitor information (SNR, Noise, signal level) is available.") );
    consoleDir()
        .add("sndBuf", fty::Command(
                 SENF_MEMBINDFNP(void, HardwareWLANInterface, sndBuf, (unsigned)))
             .doc( "set the send socket buffer size in bytes"));
    consoleDir()
        .add("sndBuf", fty::Command(
                 SENF_MEMBINDFNP(unsigned, HardwareWLANInterface, sndBuf, ()))
             .doc( "get the send socket buffer size in bytes"));
    consoleDir()
        .add("rcvBuf", fty::Command(
                 SENF_MEMBINDFNP(void, HardwareWLANInterface, rcvBuf, (unsigned)))
             .doc( "set the receive socket buffer size in bytes"));
    consoleDir()
        .add("rcvBuf", fty::Command(
                 SENF_MEMBINDFNP(unsigned, HardwareWLANInterface, rcvBuf, ()))
             .doc( "get the receive socket buffer size in bytes"));
    consoleDir()
        .add("qlen", fty::Command(
                 SENF_MEMBINDFNP(void, HardwareWLANInterface, qlen, (unsigned)))
             .doc("set mmap read queue length in # of packets"));
    consoleDir()
        .add("qlen", fty::Command(
                 SENF_MEMBINDFNP(unsigned, HardwareWLANInterface, qlen, () const))
             .doc("get mmap read queue length in # of packets"));
    consoleDir()
        .add("maxBurst", fty::Command(
                 SENF_MEMBINDFNP(void, HardwareWLANInterface, maxBurst, (unsigned)))
             .doc("set max. number to frames to be read in a burst."));
    consoleDir()
        .add("maxBurst", fty::Command(
                 SENF_MEMBINDFNP(unsigned, HardwareWLANInterface, maxBurst, () const))
             .doc("set max. number to frames to be read in a burst."));
    consoleDir()
        .add("spectralScanStats", fty::Command(&HardwareWLANInterface::spectralScanStats, this)
             .doc("current spectralScanner stats."));

    console::provideDirectory(interfaceDir(), "by-device")
        .add(device(), fty::Link(consoleDir()));

    if (enabled())
        init_sockets();

    if (!spectralScanner_.detected()) {
        SENF_LOG( (WlanLogArea) (senf::log::MESSAGE) ("ath9k spectral scanner not detected for " << wnlc_.phyName() << ". DebugFS mounted at /sys/kernel/debug ?") );
    } else {
        SENF_LOG( (WlanLogArea) (senf::log::MESSAGE) ("ath spectral scanner detected for " << wnlc_.phyName()) );
    }
}

prefix_ void senf::emu::HardwareWLANInterface::registerFrequencies()
{
    typedef boost::icl::interval_set<WirelessNLController::frequency_type> FrequencyRanges;
    typedef boost::icl::interval<WirelessNLController::frequency_type>::type FrequencyRange;
    FrequencyRanges frequencyRanges;
    WirelessNLController::FrequencyRange nlFreqRange;
    if (restrictedBand_ < 0)
        nlFreqRange = wnlc_.frequencies();
    else
        nlFreqRange = wnlc_.frequencies( WirelessNLController::Band_t(restrictedBand_));
    // register non-HT frequencies/bandwidth
    BOOST_FOREACH( WirelessNLController::frequency_type freq, nlFreqRange ) {
        // TODO: (we need to think about this in more detail): filter out channels where the center frequency is smaller than the offset
        if ((frequencyOffset_ < 0) && (freq < unsigned(abs(frequencyOffset_))))
            continue;
        frequencyRanges.insert( FrequencyRange(
                freq + frequencyOffset_ - MHZ_TO_KHZ(10), freq + frequencyOffset_ + MHZ_TO_KHZ(10)));
        if (htMode_ != HTMode::HT40only)
            registerFrequency(freq + frequencyOffset_, MHZ_TO_KHZ(20));
    }
    if (! (wnlc_.hasHTCapabilities() && wnlc_.htCapabilities().channelWidth == HTCapabilitiesInfo::HT20_40))
        return;
    if (htMode_ == HTMode::Disabled || htMode_ == HTMode::HT20only)
        return;
    // register HT40(+) frequencies/bandwidth
    BOOST_FOREACH( WirelessNLController::frequency_type freq, wnlc_.frequencies() ) {
        if (boost::icl::contains(frequencyRanges, FrequencyRange(
                freq + frequencyOffset_ - MHZ_TO_KHZ(10), freq + frequencyOffset_ + MHZ_TO_KHZ(30)))) {
            registerFrequency(freq + frequencyOffset_ + MHZ_TO_KHZ(10), MHZ_TO_KHZ(40));
        }
    }
}

prefix_ void senf::emu::HardwareWLANInterface::registerModulations()
{
    WLANModulationParameterRegistry const & registry (WLANModulationParameterRegistry::instance());
    // register the two 'special' modulations
    registerModulation( registry.parameterIdUnknown());
    registerModulation( registry.parameterIdAuto());
    BitrateParameters const & bitrates (wnlc_.bitrates());
    if (htMode_  == HTMode::Disabled || htMode_ == HTMode::Enabled) {  // register non-HT legacy modulations
        std::set<BitrateParameters::LegacyBitrate> legacyRates;
        if (bitrates.legacy_24)
            legacyRates.insert( bitrates.legacy_24->begin(), bitrates.legacy_24->end());
        if (bitrates.legacy_5)
            legacyRates.insert( bitrates.legacy_5->begin(), bitrates.legacy_5->end());
        BOOST_FOREACH( BitrateParameters::LegacyBitrate legacyRate, legacyRates) {
            registerModulation( registry.parameterIdByLegacyRate( legacyRate));
        }
    }
    if (htMode_ != HTMode::Disabled) {  // register HT MCS modulations
        HTCapabilitiesInfo const & htCapa (wnlc_.htCapabilities());
        std::set<BitrateParameters::MCSIndex> mcsIndexes;
        if (bitrates.mcs_24)
            mcsIndexes.insert( bitrates.mcs_24->begin(), bitrates.mcs_24->end());
        if (bitrates.mcs_5)
            mcsIndexes.insert( bitrates.mcs_5->begin(), bitrates.mcs_5->end());
        BOOST_FOREACH( BitrateParameters::MCSIndex mcsIndex, mcsIndexes) {
            if (htMode_ != HTMode::HT40only)
                registerModulation( registry.parameterIdByMCS( mcsIndex, false, false));
            if (! wnlc_.hasHTCapabilities())
                continue;
            if (htMode_ != HTMode::HT40only)
                if (htCapa.rxHT20SGI) // Indicates short GI support for 20 MHz
                    registerModulation( registry.parameterIdByMCS( mcsIndex, false, true));
            if (htMode_ == HTMode::HT20only)
                continue;
            if (htCapa.channelWidth == HTCapabilitiesInfo::HT20_40) {
                registerModulation( registry.parameterIdByMCS( mcsIndex, true, false));
                if (htCapa.rxHT40SGI) // Indicates short GI support for 40 MHz
                    registerModulation( registry.parameterIdByMCS( mcsIndex, true, true));
            }
        }
    }
}

prefix_ void senf::emu::HardwareWLANInterface::refreshProperties()
{
    clearRegisteredFrequencies();
    clearRegisteredModulations();
    registerFrequencies();
    registerModulations();
}

prefix_ void senf::emu::HardwareWLANInterface::registerTxPowers()
{
    registerTxPower(100, 3000);
}

prefix_ std::string const & senf::emu::HardwareWLANInterface::device()
    const
{
    return dev_;
}

prefix_ std::string const & senf::emu::HardwareWLANInterface::monitorDevice()
    const
{
    return monitorDev_;
}

prefix_ void senf::emu::HardwareWLANInterface::init_sockets()
{
    monitorDataFilter.id(self().id());

    NetdeviceController ctrl (monitorDevice());
    ctrl.promisc(true);
    ctrl.up();

    netctl_.promisc(false);
    netctl_.down();
}

prefix_ void senf::emu::HardwareWLANInterface::openMonitorSocket()
{
    ConnectedMMapReadPacketSocketHandle monSocket_ (monitorDevice(), qlen_, SENF_EMU_MAXMTU);
    monSocket_.protocol().rcvbuf( rcvBufSize_);
    MonitorDataFilter::filterMonitorTxFrames(monSocket_);
    monitorDataFilter.promisc(true);
    monitorDataFilter.flushQueues();
    assignMonitorSocket(monSocket_);

    // in monitor mode, we disable the wifi_statistics module
    wifiStatistics_.enable(false);
}

prefix_ void senf::emu::HardwareWLANInterface::closeMonitorSocket()
{ 
    if (HardwareWLANInterfaceNet::monSocket.valid())
        HardwareWLANInterfaceNet::monSocket.close();

    assignMonitorSocket(HardwareWLANInterfaceNet::monSocket);

    // in non-monitor mode, we enable the wifi_statistics module
    wifiStatistics_.enable(true);
}

prefix_ void senf::emu::HardwareWLANInterface::openDataSocket()
{ 
    netctl_.up();
    ConnectedMMapPacketSocketHandle socket_ (device(), qlen_, SENF_EMU_MAXMTU);
    socket_.protocol().sndbuf( sndBufSize_);
    socket_.protocol().rcvbuf( rcvBufSize_);
    HardwareWLANInterfaceNet::assignDataSocket(socket_);   
}

prefix_ bool senf::emu::HardwareWLANInterface::cellJoined()
    const
{
    return HardwareWLANInterfaceNet::socket.valid();
}

prefix_ void senf::emu::HardwareWLANInterface::closeDataSocket()
{ 
    if (HardwareWLANInterfaceNet::socket.valid())
        HardwareWLANInterfaceNet::socket.close();
    
    HardwareWLANInterfaceNet::assignDataSocket(HardwareWLANInterfaceNet::socket);
    netctl_.down();
}

prefix_ void senf::emu::HardwareWLANInterface::close_sockets()
{
    closeMonitorSocket();
    closeDataSocket();
    NetdeviceController(monitorDevice()).down();    
}

prefix_ void senf::emu::HardwareWLANInterface::v_enable()
{
    if (! enabled()) {
        init_sockets();
    }
}

prefix_ void senf::emu::HardwareWLANInterface::v_disable()
{
    close_sockets();
}

prefix_ bool senf::emu::HardwareWLANInterface::v_enabled()
    const
{
    return NetdeviceController(monitorDevice()).isUp();
}

prefix_ void senf::emu::HardwareWLANInterface::v_id(MACAddress const & id)
{
    DisableInterfaceGuard guard (*this);
    netctl_.hardwareAddress( id);
    monitorDataFilter.id(id);
}

prefix_ senf::MACAddress senf::emu::HardwareWLANInterface::v_id()
    const
{
    return netctl_.hardwareAddress();
}

prefix_ bool senf::emu::HardwareWLANInterface::v_promisc()
    const
{
    return promisc_;
}

prefix_ void senf::emu::HardwareWLANInterface::v_promisc(bool p)
{
    if (promisc_ == p)
        return;

    promisc_ = p;

    if (p) {
        openMonitorSocket();
        dataSource(false);
    } else {
        closeMonitorSocket();
        dataSource(true);
    }

    frequencyHint();
}

prefix_ void senf::emu::HardwareWLANInterface::v_annotationMode(bool a)
{
    monitorDataFilter.annotate(a);
}

prefix_ bool senf::emu::HardwareWLANInterface::v_annotationMode()
    const
{
    return monitorDataFilter.annotate();
}

prefix_ void senf::emu::HardwareWLANInterface::v_mcAdd(MACAddress const & address)
{
    HardwareWLANInterfaceNet::socket.protocol().mcAdd( dev_, address);
}

prefix_ void senf::emu::HardwareWLANInterface::v_mcDrop(MACAddress const & address)
{
    HardwareWLANInterfaceNet::socket.protocol().mcDrop( dev_, address);
}

prefix_ unsigned senf::emu::HardwareWLANInterface::v_mtu()
    const
{
    return netctl_.mtu();
}

prefix_ void senf::emu::HardwareWLANInterface::v_mtu(unsigned v)
{
    netctl_.mtu(v);
    NetdeviceController(monitorDevice()).mtu(v);
}

prefix_ void senf::emu::HardwareWLANInterface::v_flushRxQueues()
{
    HardwareWLANInterfaceNet::flush();
}

prefix_ void senf::emu::HardwareWLANInterface::v_flushTxQueues()
{
    HardwareWLANInterfaceNet::sink.flush();
}

prefix_ senf::emu::ModulationParameter::id_t senf::emu::HardwareWLANInterface::v_modulationId()
    const
{
    return modId_;
}

prefix_ unsigned senf::emu::HardwareWLANInterface::v_bandwidth()
    const
{
    return bw_;
}

prefix_ void senf::emu::HardwareWLANInterface::v_modulationId(ModulationParameter::id_t id)
{
#define insertParameterIfTypeMatch(typeMatch, colType, col, value)       \
        col.reset( colType());                                           \
        if (typeMatch) col->insert( value);

    WLANModulationParameter const & modPara (WLANModulationParameterRegistry::instance().findModulationById(id));
    BitrateParameters bratePara;

    // @tho: There must be a better way to decide which mcs/legacy modualtion to provide for either 2.4 or 5.x GHz bands
    if ((wnlc_.supportedBands().size() == 1) or (wnlc_.frequency() < 3000000)) { 
        if (senf::contains(wnlc_.supportedBands(), WirelessNLController::BAND_2GHZ)) {
            insertParameterIfTypeMatch( modPara.type == WLANModulationParameter::Legacy,
                                        BitrateParameters::LegacyBitrateSet, bratePara.legacy_24, modPara.rate);
            insertParameterIfTypeMatch( modPara.type == WLANModulationParameter::MCS,
                                        BitrateParameters::MCSIndexSet, bratePara.mcs_24, modPara.mcsIndex);
        }
    }
    if ((wnlc_.supportedBands().size() == 1) or (wnlc_.frequency() > 5000000)) {
        if (senf::contains(wnlc_.supportedBands(), WirelessNLController::BAND_5GHZ)) {
            insertParameterIfTypeMatch( modPara.type == WLANModulationParameter::Legacy,
                                        BitrateParameters::LegacyBitrateSet, bratePara.legacy_5, modPara.rate);
            insertParameterIfTypeMatch( modPara.type == WLANModulationParameter::MCS,
                                        BitrateParameters::MCSIndexSet, bratePara.mcs_5, modPara.mcsIndex);
        }
    }

    wnlc_.set_bitrates(bratePara);
    modId_ = modPara.id;

#undef insertParameterIfTypeMatch
}

prefix_ void senf::emu::HardwareWLANInterface::v_frequency(unsigned freq, unsigned bandwidth)
{
    WirelessNLController wnlc (monitorDevice());

    if (freq != 0 && bandwidth != 0) {
        switch (bandwidth) {
        case MHZ_TO_KHZ(20):
            wnlc.set_frequency( freq-frequencyOffset_,
                    htMode_ == HTMode::Disabled ? WirelessNLController::ChannelMode::NoHT20 : WirelessNLController::ChannelMode::HT20);
            bw_ = MHZ_TO_KHZ(20);
            break;
        case MHZ_TO_KHZ(40):
            wnlc.set_frequency( freq-frequencyOffset_-MHZ_TO_KHZ(10), WirelessNLController::ChannelMode::HT40Plus);
            bw_ = MHZ_TO_KHZ(40);
            break;
        default:
            throw InvalidArgumentException("invalid bandwidth: ") << bandwidth;
            break;
        }
    }

    frequencyHint();

    // drop all old frames from the kernel queues
    flushRxQueues();
}

prefix_ unsigned senf::emu::HardwareWLANInterface::v_frequency()
    const
{
    switch (bw_) {
    case MHZ_TO_KHZ(20):
        return wnlc_.frequency() + frequencyOffset_;
    case MHZ_TO_KHZ(40):
        return wnlc_.frequency() + frequencyOffset_ + MHZ_TO_KHZ(10);
    }
    return 0;
}

prefix_ int senf::emu::HardwareWLANInterface::v_txPower()
    const
{
    return txPower_;
}

prefix_ void senf::emu::HardwareWLANInterface::v_txPower(int power)
{
    if (power == txPower_)
        return;

    try {
        wnlc_.set_txpower(WirelessNLController::TxPowerSetting::Fixed, power);
    } catch (NetlinkException & ex) {
        if (ex.errorNumber() != -7)  // Invalid parameter
            throw;
    }
    txPower_ = power;
}

prefix_ senf::emu::MonitorDataFilterStatistics senf::emu::HardwareWLANInterface::filterStats()
{
    return HardwareWLANInterfaceNet::monitorDataFilter.stats();
}

prefix_ void senf::emu::HardwareWLANInterface::monitorDropUnknownMCS(bool q)
{
    HardwareWLANInterfaceNet::monitorDataFilter.dropUnknownMCS(q);
}

prefix_ void senf::emu::HardwareWLANInterface::dumpFilterStats(std::ostream & os)
{
    HardwareWLANInterfaceNet::monitorDataFilter.stats().dump(os);
    HardwareWLANInterfaceNet::monitorDataFilter.dumpState(os);
}

prefix_ void senf::emu::HardwareWLANInterface::dumpMmapStats(std::ostream & os)
{
    if (!HardwareWLANInterfaceNet::socket and !HardwareWLANInterfaceNet::monSocket) {
         os << "Socket closed. No stats available." << std::endl;
         return;
    }

    if (HardwareWLANInterfaceNet::monSocket) {
        auto rs (HardwareWLANInterfaceNet::monSocket.protocol().rxStats());
        os << "MMAP Rx (monitor) stats: "; rs.dump(os);
    } else {
        auto rs (HardwareWLANInterfaceNet::socket.protocol().rxStats());
        os << "MMAP Rx (data) stats: "; rs.dump(os);
    }
    if (HardwareWLANInterfaceNet::socket) {
        auto ts (HardwareWLANInterfaceNet::socket.protocol().txStats());
        os << " MMAP Tx (data) stats: "; ts.dump(os);
    }

    os << std::endl;
}

prefix_ void senf::emu::HardwareWLANInterface::frequencyOffset(int offset)
{
    if (offset == frequencyOffset_)
        return;
    frequencyOffset_ = offset;
    clearRegisteredFrequencies();
    registerFrequencies();
}

prefix_ void senf::emu::HardwareWLANInterface::restrictBand(WirelessNLController::Band_t band)
{
    restrictedBand_ = band;
    clearRegisteredFrequencies();
    registerFrequencies();
}

prefix_ void senf::emu::HardwareWLANInterface::restrictBand()
{
    restrictedBand_ = -1;
    clearRegisteredFrequencies();
    registerFrequencies();
}

prefix_ senf::emu::HardwareWLANInterface::HTMode::Enum senf::emu::HardwareWLANInterface::htMode()
    const
{
    return htMode_;
}

prefix_ void senf::emu::HardwareWLANInterface::htMode(HTMode::Enum mode)
{
    if (mode != HTMode::Disabled && !wnlc_.hasHTCapabilities())
        throw InvalidArgumentException("HT mode not supported by this interface");

    if (htMode_ == mode)
        return;

    htMode_ = mode;
    clearRegisteredFrequencies();
    clearRegisteredModulations();
    registerFrequencies();
    registerModulations();
}

prefix_ void senf::emu::HardwareWLANInterface::v_coverageRange(unsigned distance)
{
    wnlc_.coverageClass( WirelessNLController::distanceToCoverageClass(distance));
}

prefix_ senf::emu::WirelessNLController::IbssJoinParameters::ptr
senf::emu::HardwareWLANInterface::joinAdhoc(std::string const & ssid,
		unsigned int freq, unsigned int bandwidth)
{
    if (! enabled())
        throw senf::SystemException(
            "joinAdhoc (" + ssid + ") failed: Interface (" + device() + ") is down ", ENETDOWN);

    WirelessNLController::ChannelMode::Enum channelMode;
    switch (bandwidth) {
    case MHZ_TO_KHZ(20):
		channelMode = (htMode_ == HTMode::Disabled
                       ? WirelessNLController::ChannelMode::NoHT20
                       : WirelessNLController::ChannelMode::HT20);
        break;
    case MHZ_TO_KHZ(40):
		channelMode = WirelessNLController::ChannelMode::HT40Plus;
        freq -= MHZ_TO_KHZ(10);
        break;
    default:
        throw InvalidArgumentException("invalid bandwidth: ") << bandwidth;
    }

    return WirelessNLController::IbssJoinParameters::ptr( new WirelessNLController::IbssJoinParameters(
            membind(&HardwareWLANInterface::do_ibss_join, this),
            ssid, freq-frequencyOffset_, channelMode) );
}

prefix_ senf::emu::WirelessNLController::MeshJoinParameters::ptr
senf::emu::HardwareWLANInterface::joinMesh(std::string const & meshId,
		unsigned int freq, unsigned int bandwidth)
{
    if (! enabled())
        throw senf::SystemException(
            "joinMesh (" + meshId + ") failed: Interface (" + device() + ") is down ", ENETDOWN);

    WirelessNLController::ChannelMode::Enum channelMode;
    switch (bandwidth) {
    case MHZ_TO_KHZ(20):
		channelMode = (htMode_ == HTMode::Disabled
                       ? WirelessNLController::ChannelMode::NoHT20
                       : WirelessNLController::ChannelMode::HT20);
        break;
    case MHZ_TO_KHZ(40):
		channelMode = WirelessNLController::ChannelMode::HT40Plus;
        freq -= MHZ_TO_KHZ(10);
        break;
    default:
        throw InvalidArgumentException("invalid bandwidth: ") << bandwidth;
    }

    return WirelessNLController::MeshJoinParameters::ptr( new WirelessNLController::MeshJoinParameters(
            membind(&HardwareWLANInterface::do_mesh_join, this),
            meshId, freq-frequencyOffset_, channelMode) );
}

prefix_ void senf::emu::HardwareWLANInterface::do_mesh_join(WirelessNLController::MeshJoinParameters const & parameters)
{
    openDataSocket();
    
    wnlc_.do_mesh_join(parameters);
    bw_ = parameters.channelMode_ == WirelessNLController::ChannelMode::HT40Plus ? MHZ_TO_KHZ(40) : MHZ_TO_KHZ(20);

    frequencyHint();
}

prefix_ void senf::emu::HardwareWLANInterface::do_ibss_join(WirelessNLController::IbssJoinParameters const & parameters)
{
    openDataSocket();
    
    wnlc_.do_ibss_join(parameters);
    bw_ = parameters.channelMode_ == WirelessNLController::ChannelMode::HT40Plus ? MHZ_TO_KHZ(40) : MHZ_TO_KHZ(20);

    frequencyHint();
}

prefix_ void senf::emu::HardwareWLANInterface::leaveCell()
{
    try {
        wnlc_.mesh_leave();
    }
    catch (...) {};

    try {
        wnlc_.ibss_leave();
    } catch (...) {};

    closeDataSocket();
}

prefix_ void senf::emu::HardwareWLANInterface::sndBuf(unsigned sndbuf)
{
    // need to cache sndBufSize_ so we can (re-)apply the value in v_enable()
    sndBufSize_ = std::max(sndbuf, 2048u);
    HardwareWLANInterfaceNet::sndBuf( sndBufSize_);
}

prefix_ unsigned senf::emu::HardwareWLANInterface::sndBuf()
{
    // need to cache sndBufSize_ so we can (re-)apply the value in v_enable()
    return sndBufSize_ = HardwareWLANInterfaceNet::sndBuf();
}

prefix_ void senf::emu::HardwareWLANInterface::rcvBuf(unsigned rcvbuf)
{
    // need to cache rcvBufSize_ so we can (re-)apply the value in v_enable()
    rcvBufSize_ = std::max(rcvbuf, 4096u);
    HardwareWLANInterfaceNet::rcvBuf( rcvBufSize_);
}

prefix_ unsigned senf::emu::HardwareWLANInterface::rcvBuf()
{
    // need to cache rcvBufSize_ so we can (re-)apply the value in v_enable()
    return rcvBufSize_ = HardwareWLANInterfaceNet::rcvBuf();
}

prefix_ unsigned senf::emu::HardwareWLANInterface::qlen()
    const
{
    return qlen_;
}

prefix_ void senf::emu::HardwareWLANInterface::qlen(unsigned qlen)
{
    if (qlen == qlen_)
        return;
    qlen_ = qlen;
    if (enabled()) {
        close_sockets();
        init_sockets();
    }
}

prefix_ std::pair<unsigned,unsigned> senf::emu::HardwareWLANInterface::rxDropped()
{
    if (HardwareWLANInterfaceNet::monSocket) {
        return monSource.dropped();
    }

    return source.dropped();
}

prefix_ std::pair<unsigned,unsigned> senf::emu::HardwareWLANInterface::txDropped()
{
    return sink.dropped();
}

prefix_ unsigned senf::emu::HardwareWLANInterface::maxBurst()
    const
{
    return source.maxBurst();
}

prefix_ void senf::emu::HardwareWLANInterface::maxBurst(unsigned maxBurst)
{
    source.maxBurst(maxBurst);
}

prefix_ void senf::emu::HardwareWLANInterface::frequencyHint()
{
    unsigned freq (wnlc_.frequency() / 1000);
    spectralScanner_.frequency(freq, frequencyOffset() / 1000);
    monitorDataFilter.frequency(freq);
}

prefix_ void senf::emu::HardwareWLANInterface::spectralScanCallback(AthSpectralScan::AthSpectralScanCallback const & cb)
{
    spectralScanner_.callback(cb);
}

prefix_ void senf::emu::HardwareWLANInterface::spectralScanCallback()
{
    spectralScanner_.disable();
}

prefix_ void senf::emu::HardwareWLANInterface::spectralScanStats(std::ostream & os)
{
    spectralScanner_.stats(os);
}

senf::emu::WifiStatisticsMap const & senf::emu::HardwareWLANInterface::statisticsMap(std::uint32_t tag)
{
    return wifiStatistics_.statisticsMap(tag);
}

prefix_ senf::emu::WirelessNLController::DFSState::Enum senf::emu::HardwareWLANInterface::dfsState(unsigned freq, unsigned bandwidth)
{
    WirelessNLController wnlc (monitorDev_);

    if (freq == 0 or bandwidth == 0)
        return WirelessNLController::DFSState::NoDFS;

    if (bandwidth == MHZ_TO_KHZ(20))
        return wnlc.dfsState(freq - frequencyOffset_);

    if (bandwidth == MHZ_TO_KHZ(40)) {
        /*             | noDFS  | usable    | available | unavailable
         *       noDFS | noDFS  | usable    | available | unavailable
         *      usable |        | usable    | usable    | unavailable
         *   available |        |           | available | unavailable
         * unavailable |        |           |           | unavailable
         */
        WirelessNLController::DFSState::Enum state1 (wnlc.dfsState(freq - frequencyOffset_ - MHZ_TO_KHZ(10)));
        WirelessNLController::DFSState::Enum state2 (wnlc.dfsState(freq - frequencyOffset_ + MHZ_TO_KHZ(10)));
        if (state1 == WirelessNLController::DFSState::NoDFS and state2 == WirelessNLController::DFSState::NoDFS)
            return WirelessNLController::DFSState::NoDFS;
        if (state1 == WirelessNLController::DFSState::Unavailable or state2 == WirelessNLController::DFSState::Unavailable)
            return WirelessNLController::DFSState::Unavailable;
        if (state1 == WirelessNLController::DFSState::Usable or state2 == WirelessNLController::DFSState::Usable)
            return WirelessNLController::DFSState::Usable;
        return WirelessNLController::DFSState::Available;
    }

    throw InvalidArgumentException("invalid bandwidth: ") << bandwidth;
}

#undef MHZ_TO_KHZ
#undef IGNORE_EXCPETION

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
