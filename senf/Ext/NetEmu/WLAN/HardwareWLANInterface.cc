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


   // @tho: There must be a better way to decide which mcs/legacy modualtion to provide for either 2.4 or 5.x GHz bands

/** \file
    \brief HardwareWLANInterface non-inline non-template implementation */

#include "HardwareWLANInterface.hh"

// Custom includes
#include <linux/filter.h>
#include <algorithm>
#include <boost/icl/interval_set.hpp>
#include <senf/Utils/algorithm.hh>
#include <senf/Utils/String.hh>
#include <senf/Packets/80211Bundle/MCSInfo.hh>
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
      annotatorRx_(true, true), annotatorTx_(false, true),
      monSocket (senf::noinit),
      monSource(monSocket),
      netOutput (monitorDataFilter.output), netInput (annotatorTx_.input)
{
    ppi::connect(source, annotatorRx_.input);
    ppi::connect(annotatorRx_.output, monitorDataFilter.input_plain);
    ppi::connect(monSource, monitorDataFilter);
    ppi::connect(annotatorTx_.output, sink.input);
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
      wifiStatistics_(monitorDevice()), frequencyOffset_(0), restrictedBand_(-1), htMode_(HTMode::Disabled),
      modId_( WLANModulationParameterRegistry::instance().parameterIdUnknown()), bw_(0), txPower_(0),
      rcvBufSize_ (1024), sndBufSize_ (96*1024), qlen_ (512), pvid_(VLanId::None), accessMode_(false)
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
        .add("pvid", fty::Command(
                 SENF_MEMBINDFNP(bool, HardwareWLANInterface, pvid, (VLanId const &, bool)))
             .doc( "enables filtering for a specific PVID (VLAN ID must be 0...4095)"));
    consoleDir()
        .add("pvid", fty::Command(
                 SENF_MEMBINDFNP(VLanId const &, HardwareWLANInterface, pvid, () const))
             .doc( "report the currently configured PVID (-1 means none)"));
    consoleDir()
        .add("spectralScanStats", fty::Command(&HardwareWLANInterface::spectralScanStats, this)
             .doc("current spectralScanner stats."));
    consoleDir()
        .add("dumpSurvey", fty::Command(&HardwareWLANInterface::dumpSurvey, this)
             .doc("current survey stats"));
    consoleDir()
        .add("triggerScan", fty::Command(&HardwareWLANInterface::triggerScan, this)
             .doc("triggers a fresh scan"));
    consoleDir()
        .add("getScan", fty::Command(&HardwareWLANInterface::getScan, this)
             .doc("retrieves latest scan results"));
    consoleDir()
        .add("wifiStatistics", fty::Command(&HardwareWLANInterface::dumpWifiStatistics, this)
             .doc("Dumps current WifiStatistics."));

    console::provideDirectory(interfaceDir(),"by-device").add(device(), fty::Link(consoleDir()));

    if (enabled())
        init_sockets();
    
    if (!spectralScanner_.detected()) {
        SENF_LOG( (WlanLogArea) (senf::log::MESSAGE) ("ath spectral scanner not detected for " << wnlc_.phyName() << ". DebugFS mounted at /sys/kernel/debug ?") );
    } else {
        SENF_LOG( (WlanLogArea) (senf::log::MESSAGE) ("ath spectral scanner detected for " << wnlc_.phyName()) );
    }
}

prefix_ senf::emu::HardwareWLANInterface::~HardwareWLANInterface()
{
    try {
        console::provideDirectory(interfaceDir(),"by-device").remove(device());
    } catch (...) {};
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
    for (WirelessNLController::frequency_type freq : nlFreqRange) {
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
    for (WirelessNLController::frequency_type freq : wnlc_.frequencies()) {
        if (boost::icl::contains(frequencyRanges, FrequencyRange(
                freq + frequencyOffset_ - MHZ_TO_KHZ(10), freq + frequencyOffset_ + MHZ_TO_KHZ(30)))) {
            registerFrequency(freq + frequencyOffset_ + MHZ_TO_KHZ(10), MHZ_TO_KHZ(40));
        }
    }
    if (htMode_ == HTMode::HT40only || !wnlc_.hasVHTCapabilities())
        return;
    // register VHT80( frequencies/bandwidth
    for (WirelessNLController::frequency_type freq : wnlc_.frequencies()) {
        if (boost::icl::contains(frequencyRanges, FrequencyRange(
                freq + frequencyOffset_ - MHZ_TO_KHZ(50), freq + frequencyOffset_ + MHZ_TO_KHZ(30)))) {
            registerFrequency(freq + frequencyOffset_ - MHZ_TO_KHZ(10), MHZ_TO_KHZ(80));
        }
    }
}

prefix_ void senf::emu::HardwareWLANInterface::registerVHTModulation(unsigned vhtMcsIndex, unsigned nss, unsigned bw, bool shortGI)
{
    WLANModulationParameterRegistry const & registry (WLANModulationParameterRegistry::instance());
    if (WLAN_MCSInfo::getRate(vhtMcsIndex, nss+1, bw, shortGI) > 0)
        registerModulation(registry.parameterIdByMCS_VHT(vhtMcsIndex, nss+1, bw, shortGI));
}

prefix_ void senf::emu::HardwareWLANInterface::registerModulations_vht(BitrateParameters::VHT_MCSBitmapTable const & vht_mcs_table)
{
    VHTCapabilitiesInfo const & vhtCapa (wnlc_.vhtCapabilities());
    for (size_t nss = 0; nss < vht_mcs_table.size(); nss++) {
        for (size_t index = 0; index < vht_mcs_table[nss].size(); index++) {
            if (not vht_mcs_table[nss].test(index))
                continue;
            for (int bw : {MHZ_TO_KHZ(20), MHZ_TO_KHZ(40)}) {
                registerVHTModulation(index, nss, bw, false);
                registerVHTModulation(index, nss, bw, true);
            }
            registerVHTModulation(index, nss, MHZ_TO_KHZ(80), false);
            if (vhtCapa.shortGIfor80MHz)
                registerVHTModulation(index, nss, MHZ_TO_KHZ(80), true);
            if (vhtCapa.supportedChannelWidth == VHTCapabilitiesInfo::SupportedChannelWidth_160MHz) {
                registerVHTModulation(index, nss, MHZ_TO_KHZ(160), false);
                if (vhtCapa.shortGIfor160_8080MHz)
                    registerVHTModulation(index, nss, MHZ_TO_KHZ(160), true);
            }
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
    if (htMode_  == HTMode::Disabled || htMode_ == HTMode::Enabled) {
        // register non-HT legacy modulations
        std::set<BitrateParameters::LegacyBitrate> legacyRates;
        if (bitrates.legacy_24)
            legacyRates.insert( bitrates.legacy_24->begin(), bitrates.legacy_24->end());
        if (bitrates.legacy_5)
            legacyRates.insert( bitrates.legacy_5->begin(), bitrates.legacy_5->end());
        for (BitrateParameters::LegacyBitrate legacyRate : legacyRates) {
            registerModulation( registry.parameterIdByLegacyRate( legacyRate));
        }
    }
    if (htMode_ != HTMode::Disabled) {
        // register HT MCS modulations
        HTCapabilitiesInfo const & htCapa (wnlc_.htCapabilities());
        std::set<BitrateParameters::MCSIndex> mcsIndexes;
        if (bitrates.mcs_24)
            mcsIndexes.insert( bitrates.mcs_24->begin(), bitrates.mcs_24->end());
        if (bitrates.mcs_5)
            mcsIndexes.insert( bitrates.mcs_5->begin(), bitrates.mcs_5->end());
        for (BitrateParameters::MCSIndex mcsIndex : mcsIndexes) {
            if (htMode_ != HTMode::HT40only)
                registerModulation( registry.parameterIdByMCS_HT( mcsIndex, MHZ_TO_KHZ(20), false));
            if (! wnlc_.hasHTCapabilities())
                continue;
            if (htMode_ != HTMode::HT40only)
                if (htCapa.rxHT20SGI) // Indicates short GI support for 20 MHz
                    registerModulation( registry.parameterIdByMCS_HT( mcsIndex, MHZ_TO_KHZ(20), true));
            if (htMode_ == HTMode::HT20only)
                continue;
            if (htCapa.channelWidth == HTCapabilitiesInfo::HT20_40) {
                registerModulation( registry.parameterIdByMCS_HT( mcsIndex, MHZ_TO_KHZ(40), false));
                if (htCapa.rxHT40SGI) // Indicates short GI support for 40 MHz
                    registerModulation( registry.parameterIdByMCS_HT( mcsIndex, MHZ_TO_KHZ(40), true));
            }
        }
        // register VHT MCS modulations
        if (wnlc_.hasVHTCapabilities()) {
            if (bitrates.vht_mcs_table_24)
                registerModulations_vht(bitrates.vht_mcs_table_24.get());
            if (bitrates.vht_mcs_table_5)
                registerModulations_vht(bitrates.vht_mcs_table_5.get());
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

prefix_ std::string const & senf::emu::HardwareWLANInterface::v_device()
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
    monitorDataFilter.id(id());
    annotatorRx_.id(id());

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
    monitorDataFilter.promisc(netctl_.promisc());
    monitorDataFilter.flushQueues();
    assignMonitorSocket(monSocket_);

    // in monitor mode, we disable the wifi_statistics module
    wifiStatistics_.enable(false);
}

prefix_ void senf::emu::HardwareWLANInterface::closeMonitorSocket()
{
    monSource.handle(ConnectedMMapReadPacketSocketHandle(senf::noinit));

    if (HardwareWLANInterfaceNet::monSocket.valid())
        HardwareWLANInterfaceNet::monSocket.close();


    // in non-monitor mode, we enable the wifi_statistics module
    wifiStatistics_.enable(true);
}

prefix_ void senf::emu::HardwareWLANInterface::openDataSocket()
{ 
    std::string vlanDevice (device() + "." + senf::str(pvid_));

    netctl_.up();
    
    if (!promisc() and pvid_) {
        // if there exists a VLAN interface, remove it first
        try {
            netctl_.delVLAN(pvid_.id());
        }
        catch (...) {
        }
        netctl_.addVLAN(pvid_.id());
        NetdeviceController(vlanDevice).up();
    }

    ConnectedMMapPacketSocketHandle socket_ (((promisc() or !pvid_) ? device() : vlanDevice),
                                             qlen_, SENF_EMU_MAXMTU);

    socket_.protocol().rcvbuf( rcvBufSize_);
    socket_.protocol().sndbuf( sndBufSize_);
    // socket_.protocol().sndLowat(SENF_EMU_MAXMTU);
    HardwareWLANInterfaceNet::assignDataSocket(socket_);   

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

prefix_ bool senf::emu::HardwareWLANInterface::cellJoined()
    const
{
    return HardwareWLANInterfaceNet::socket.valid();
}

prefix_ void senf::emu::HardwareWLANInterface::closeDataSocket()
{
    try {
        if (!promisc() and pvid_) {
            netctl_.delVLAN(pvid_.id());
        }
    }
    catch (...) {
    }

    source.handle(ConnectedMMapPacketSocketHandle(senf::noinit));
    sink.handle(ConnectedMMapPacketSocketHandle(senf::noinit));
    
    if (HardwareWLANInterfaceNet::socket.valid())
        HardwareWLANInterfaceNet::socket.close();
    
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
    netctl_.hardwareAddress(id);
    monitorDataFilter.id(id);
    annotatorRx_.id(id);
}

prefix_ senf::MACAddress senf::emu::HardwareWLANInterface::v_id()
    const
{
    return netctl_.hardwareAddress();
}

prefix_ bool senf::emu::HardwareWLANInterface::v_promisc()
    const
{
    return netctl_.promisc();
}

prefix_ void senf::emu::HardwareWLANInterface::v_promisc(bool p)
{
    if ( netctl_.promisc() == p)
        return;

    // NOTE: If a PVID is configured, we need to close/open the socket
    // Hence, we would loose any join mesh/ibss states.
    
    bool dataSocketOpen (HardwareWLANInterfaceNet::socket.valid());
    if (dataSocketOpen and pvid_) {
        closeDataSocket();
    }

    netctl_.promisc(p);
    monitorDataFilter.promisc(p);

    if (dataSocketOpen and pvid_) {
        openDataSocket();
    }

    frequencyHint(0);
}

prefix_ void senf::emu::HardwareWLANInterface::v_annotationMode(bool a)
{
    if (monitorDataFilter.annotate() == a)
        return;
    
    monitorDataFilter.annotate(a);

    if (a) {
        openMonitorSocket();
        dataSource(false);
    } else {
        closeMonitorSocket();
        dataSource(true);
    }
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

prefix_ bool senf::emu::HardwareWLANInterface::pvid(VLanId const & p, bool accessMode)
{
    if (!accessMode and p.stag())
        return false;

    if (pvid_ == p and accessMode_ == accessMode)
        return true;

    // NOTE: Here we loose any join mesh/ibss states.

    bool dataSocketOpen (HardwareWLANInterfaceNet::socket.valid());
    if (dataSocketOpen) {
        closeDataSocket();
    }

    pvid_ = p;
    accessMode_ = accessMode;

    if (dataSocketOpen) {
        openDataSocket();
    }
    
    return true;
}

prefix_ std::uint32_t senf::emu::HardwareWLANInterface::vlanMismatchRx()
{
    return HardwareWLANInterfaceNet::annotatorRx_.vlanMismatch();
}

prefix_ std::uint32_t senf::emu::HardwareWLANInterface::vlanMismatchTx()
{
    return HardwareWLANInterfaceNet::annotatorTx_.vlanMismatch();
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
    WLANModulationParameter const & modPara (WLANModulationParameterRegistry::instance().findModulationById(id));
    BitrateParameters bratePara;

    if ((wnlc_.supportedBands().size() == 1) or (wnlc_.frequency() <= 3000000)) { 
        if (senf::contains(wnlc_.supportedBands(), WirelessNLController::BAND_2GHZ)) {
            switch (modPara.type) {
            case WLANModulationParameter::Automatic:
            case WLANModulationParameter::VHT:
                bratePara.vht_mcs_table_24.reset(BitrateParameters::VHT_MCSBitmapTable());
                // fall through
            case WLANModulationParameter::HT:
                bratePara.mcs_24.reset(BitrateParameters::MCSIndexSet());
                // fall through
            case WLANModulationParameter::Legacy:
		if (modPara.type != WLANModulationParameter::HT and modPara.type != WLANModulationParameter::VHT)
                    bratePara.legacy_24.reset(BitrateParameters::LegacyBitrateSet());
            case WLANModulationParameter::Unknown:
                break;
            }
            if (modPara.type == WLANModulationParameter::Legacy) {
                bratePara.legacy_24->insert(modPara.rate);
            }
            if (modPara.type == WLANModulationParameter::HT) {
                bratePara.mcs_24->insert(modPara.index);
            }
            if (modPara.type == WLANModulationParameter::VHT) {
                // ath10k requires bits 0...7 to be set. Also there can not be gaps.
                for (unsigned n = 1; n <= std::max(8u,modPara.index); n++)
                    bratePara.vht_mcs_table_24->at(modPara.streams-1).set(n-1);
                bratePara.vht_mcs_table_24->at(modPara.streams-1).set(modPara.index);
            }
        }
    }
    if ((wnlc_.supportedBands().size() == 1) or (wnlc_.frequency() >= 4900000)) {
        if (senf::contains(wnlc_.supportedBands(), WirelessNLController::BAND_5GHZ)) {
            switch (modPara.type) {
            case WLANModulationParameter::Automatic:
            case WLANModulationParameter::VHT:
                bratePara.vht_mcs_table_5.reset(BitrateParameters::VHT_MCSBitmapTable());
                // fall through
            case WLANModulationParameter::HT:
                bratePara.mcs_5.reset(BitrateParameters::MCSIndexSet());
                // fall through
            case WLANModulationParameter::Legacy:
                if (modPara.type != WLANModulationParameter::HT and modPara.type != WLANModulationParameter::VHT)
                    bratePara.legacy_5.reset(BitrateParameters::LegacyBitrateSet());
            case WLANModulationParameter::Unknown:
                break;
            }
            if (modPara.type == WLANModulationParameter::Legacy) {
                bratePara.legacy_5->insert(modPara.rate);
            }
            if (modPara.type == WLANModulationParameter::HT) {
                bratePara.mcs_5->insert(modPara.index);
            }
            if (modPara.type == WLANModulationParameter::VHT) {
                // ath10k requires bits 0...7 to be set. Also there can not be gaps.
                for (unsigned n = 1; n <= std::max(8u,modPara.index); n++)
                        bratePara.vht_mcs_table_5->at(modPara.streams-1).set(n-1);
                bratePara.vht_mcs_table_5->at(modPara.streams-1).set(modPara.index);
            }
        }
    }
    
    wnlc_.set_bitrates(bratePara);
    modId_ = modPara.id;

#undef insertParameterIfTypeMatch
}

prefix_ void senf::emu::HardwareWLANInterface::modulationSet(std::set<ModulationParameter::id_t> const & ids)
{
    BitrateParameters bratePara;
    std::set<WLANModulationParameter::Type> types;

    if (!ids.empty()) {
        try {
            for (auto const & id : ids)
                types.emplace(WLANModulationParameterRegistry::instance().findModulationById(id).type);
        }
        catch(std::exception & e) {
            throw InvalidArgumentException("Unknown WLANModulationParameter") << e.what();
        }
        if (types.size() != 1) {
            throw InvalidArgumentException("WLANModulationParameter mismatch");
        }
    
        if ((wnlc_.supportedBands().size() == 1) or (wnlc_.frequency() <= 3000000)) { 
            if (senf::contains(wnlc_.supportedBands(), WirelessNLController::BAND_2GHZ)) {
                switch (*types.begin()) {
                case WLANModulationParameter::Automatic:
                case WLANModulationParameter::VHT:
                    bratePara.vht_mcs_table_24.reset(BitrateParameters::VHT_MCSBitmapTable());
                    // fall through
                case WLANModulationParameter::HT:
                    bratePara.mcs_24.reset(BitrateParameters::MCSIndexSet());
                    // fall through
                case WLANModulationParameter::Legacy:
                    if (*types.begin() != WLANModulationParameter::HT and *types.begin() != WLANModulationParameter::VHT)
                        bratePara.legacy_24.reset(BitrateParameters::LegacyBitrateSet());
                case WLANModulationParameter::Unknown:
                    break;
                }
                if (*types.begin() == WLANModulationParameter::Legacy) {
                    for (auto const & id : ids) {
                        WLANModulationParameter const & modPara (WLANModulationParameterRegistry::instance().findModulationById(id));
                        bratePara.legacy_24->insert(modPara.rate);
                        // ath5k (at least the XR7) requires 1M to be set ?!?
                        bratePara.legacy_24->insert(1000);
                    }
                }
                if (*types.begin() == WLANModulationParameter::HT) {
                    for (auto const & id : ids) {
                        WLANModulationParameter const & modPara (WLANModulationParameterRegistry::instance().findModulationById(id));
                        bratePara.mcs_24->insert(modPara.index);
                    }
                }
                if (*types.begin() == WLANModulationParameter::VHT) {
                    // ath10k requires bits 0...7 to be set. Also there can not be gaps.
                    std::map<unsigned,unsigned> maxMCS;
                    for (auto const & id : ids) {
                        WLANModulationParameter const & modPara (WLANModulationParameterRegistry::instance().findModulationById(id));
                        maxMCS[modPara.streams-1] = std::max(7u, std::max(maxMCS[modPara.streams-1], modPara.index));
                    }
                    for (unsigned nss = 0; nss < NL80211_VHT_NSS_MAX; nss++) {
                        for (unsigned n = 0; n <= maxMCS[nss]; n++)
                            bratePara.vht_mcs_table_24->at(nss).set(n);
                    }
                }
            }
        }
        if ((wnlc_.supportedBands().size() == 1) or (wnlc_.frequency() >= 4900000)) {
            if (senf::contains(wnlc_.supportedBands(), WirelessNLController::BAND_5GHZ)) {
                switch (*types.begin()) {
                case WLANModulationParameter::Automatic:
                case WLANModulationParameter::VHT:
                    bratePara.vht_mcs_table_5.reset(BitrateParameters::VHT_MCSBitmapTable());
                    // fall through
                case WLANModulationParameter::HT:
                    bratePara.mcs_5.reset(BitrateParameters::MCSIndexSet());
                    // fall through
                case WLANModulationParameter::Legacy:
                    if (*types.begin() != WLANModulationParameter::HT and *types.begin() != WLANModulationParameter::VHT)
                        bratePara.legacy_5.reset(BitrateParameters::LegacyBitrateSet());
                case WLANModulationParameter::Unknown:
                    break;
                }
                if (*types.begin() == WLANModulationParameter::Legacy) {
                    for (auto const & id : ids) {
                        WLANModulationParameter const & modPara (WLANModulationParameterRegistry::instance().findModulationById(id));
                        bratePara.legacy_5->insert(modPara.rate);
                    }
                }
                if (*types.begin() == WLANModulationParameter::HT) {
                    for (auto const & id : ids) {
                        WLANModulationParameter const & modPara (WLANModulationParameterRegistry::instance().findModulationById(id));
                        bratePara.mcs_5->insert(modPara.index);
                    }
                }
                if (*types.begin() == WLANModulationParameter::VHT) {
                    // ath10k requires bits 0...7 to be set. Also there can not be gaps.
                    std::map<unsigned,unsigned> maxMCS;
                    for (auto const & id : ids) {
                        WLANModulationParameter const & modPara (WLANModulationParameterRegistry::instance().findModulationById(id));
                        maxMCS[modPara.streams-1] = std::max(8u, std::max(maxMCS[modPara.streams-1], modPara.index+1));
                    }
                    for (unsigned nss = 0; nss < NL80211_VHT_NSS_MAX; nss++) {
                        if (maxMCS[nss] > 0) {
                            for (unsigned n = 1; n <= maxMCS[nss]; n++)
                                bratePara.vht_mcs_table_5->at(nss).set(n-1);
                        }
                    }
                }
            }
        }
    }

    wnlc_.set_bitrates(bratePara);

    // report the upper bound (our target rate, the other(s) are considered as fallbacks)
    if (!ids.empty()) {
        std::map<unsigned, ModulationParameter::id_t> rates;
        for (auto const & i : ids) {
            rates.insert(std::make_pair(WLANModulationParameterRegistry::instance().findModulationById(i).rate, i));
        }
        modId_ = rates.rbegin()->second;
    } else {
        modId_ = WLANModulationParameterRegistry::instance().parameterIdAuto();
    }

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
        case MHZ_TO_KHZ(80):
            wnlc.set_frequency( freq-frequencyOffset_-MHZ_TO_KHZ(30), WirelessNLController::ChannelMode::VHT80);
            bw_ = MHZ_TO_KHZ(80);
            break;
        default:
            throw InvalidArgumentException("invalid bandwidth: ") << bandwidth;
            break;
        }
    }

    frequencyHint(bw_);

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
    case MHZ_TO_KHZ(80):
        return wnlc_.frequency() + frequencyOffset_ + MHZ_TO_KHZ(30);
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
    // rounding it to a dB value (ath10k seems to require this)
    power /= 100;
    power *= 100;
    
    if (power == txPower_)
        return;

    try {
        wnlc_.set_txpower(WirelessNLController::TxPowerSetting::Fixed, power);
    } catch (NetlinkException & ex) {
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
        os << "MMAP Rx (monitor, fd=" << monSocket.protocol().fd() << ", avail " << socket.protocol().available() << ") stats: "; rs.dump(os);
    } else {
        auto rs (HardwareWLANInterfaceNet::socket.protocol().rxStats());
        os << "MMAP Rx (data, fd=" << socket.protocol().fd() << ", avail " << socket.protocol().available() << ") stats: "; rs.dump(os);
    }
    if (HardwareWLANInterfaceNet::socket) {
        auto ts (HardwareWLANInterfaceNet::socket.protocol().txStats());
        os << "MMAP Tx (data, fd=" << socket.protocol().fd() << ") stats: "; ts.dump(os);
    }

    os << std::endl;
}

prefix_ bool senf::emu::HardwareWLANInterface::isDead()
    const
{
    if (HardwareWLANInterfaceNet::socket)
        return socket.protocol().interfaceDead();
    return false;  // do not call us if the iface is not active
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

prefix_ senf::emu::WirelessNLController::IbssJoinParameters::ptrI
senf::emu::HardwareWLANInterface::joinAdhoc(std::string const & ssid,
                                            unsigned int freq, unsigned int bandwidth, bool & success)
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
    case MHZ_TO_KHZ(80):
        channelMode = WirelessNLController::ChannelMode::VHT80;
        freq -= MHZ_TO_KHZ(30);
        break;
    default:
        throw InvalidArgumentException("invalid bandwidth: ") << bandwidth;
    }

    return WirelessNLController::IbssJoinParameters::ptrI(new WirelessNLController::IbssJoinParameters(
            membind(&HardwareWLANInterface::do_ibss_join, this),
            ssid, freq-frequencyOffset_, channelMode, success) );
}

prefix_ senf::emu::WirelessNLController::MeshJoinParameters::ptrM
senf::emu::HardwareWLANInterface::joinMesh(std::string const & meshId,
                                           unsigned int freq, unsigned int bandwidth, bool & success)
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
    case MHZ_TO_KHZ(80):
        channelMode = WirelessNLController::ChannelMode::VHT80;
        freq -= MHZ_TO_KHZ(30);
        break;
    default:
        throw InvalidArgumentException("invalid bandwidth: ") << bandwidth;
    }
    
    return WirelessNLController::MeshJoinParameters::ptrM(new WirelessNLController::MeshJoinParameters(
            membind(&HardwareWLANInterface::do_mesh_join, this),
            meshId, freq-frequencyOffset_, channelMode, success) );
}

prefix_ void senf::emu::HardwareWLANInterface::setCellJoined(unsigned bandwidth)
{ 
    if (bandwidth == 0) {
        closeDataSocket();
    } else {
        if (bandwidth != 20000 and bandwidth != 40000 and bandwidth != 80000) {
            throw InvalidArgumentException("invalid bandwidth: ") << bandwidth;
        } 
        openDataSocket();
        bw_ = bandwidth;
        frequencyHint(bw_);   
    }
}

prefix_ void senf::emu::HardwareWLANInterface::do_mesh_join(WirelessNLController::JoinParameters const & parameters)
{
    openDataSocket();
    
    wnlc_.do_mesh_join(parameters);

    switch (parameters.channelMode_) {
    case WirelessNLController::ChannelMode::HT40Plus:
        bw_ = MHZ_TO_KHZ(40);
        break;
    case WirelessNLController::ChannelMode::NoHT20:
    case WirelessNLController::ChannelMode::HT20:
        bw_ = MHZ_TO_KHZ(20);
        break;
    case WirelessNLController::ChannelMode::VHT80:
        bw_ = MHZ_TO_KHZ(80);
        break;
    default:
        throw InvalidArgumentException("invalid channelMode: ") << parameters.channelMode_;
        break;
    }

    frequencyHint(bw_);
}

prefix_ void senf::emu::HardwareWLANInterface::do_ibss_join(WirelessNLController::JoinParameters const & parameters)
{
    openDataSocket();
    
    wnlc_.do_ibss_join(parameters);

    switch (parameters.channelMode_) {
    case WirelessNLController::ChannelMode::HT40Plus:
        bw_ = MHZ_TO_KHZ(40);
        break;
    case WirelessNLController::ChannelMode::NoHT20:
    case WirelessNLController::ChannelMode::HT20:
        bw_ = MHZ_TO_KHZ(20);
        break;
    case WirelessNLController::ChannelMode::VHT80:
        bw_ = MHZ_TO_KHZ(80);
        break;
    default:
        throw InvalidArgumentException("invalid channelMode: ") << parameters.channelMode_;
        break;
    }

    frequencyHint(bw_);
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

prefix_ std::tuple<unsigned,unsigned,unsigned> senf::emu::HardwareWLANInterface::txDropped()
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

prefix_ void senf::emu::HardwareWLANInterface::frequencyHint(unsigned bandwidth)
{
    unsigned freq (wnlc_.frequency() / 1000);
    spectralScanner_.frequency(freq, bandwidth / 1000, frequencyOffset() / 1000);
    monitorDataFilter.frequency(freq);
}

prefix_ void senf::emu::HardwareWLANInterface::airTime(bool a)
{
    monitorDataFilter.airTime(a);
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

prefix_ senf::emu::WifiStatisticsMap const & senf::emu::HardwareWLANInterface::statisticsMap(std::uint32_t tag, senf::ClockService::clock_type const & maxAge)
{
    return wifiStatistics_.statisticsMap(tag, maxAge);
}

prefix_ void senf::emu::HardwareWLANInterface::dumpWifiStatistics(std::ostream & os)
{
    os << "=== Current (cached since last read) WifiStatsitics map, #" << wifiStatistics_.map().size() << " ===" << std::endl;
    for (auto const & m: wifiStatistics_.map()) {
        os << m.first << " ==> pkts " << m.second.total << ", lastSeen " << senf::ClockService::in_seconds(m.second.lastSeen) << "s"
           << ", signal " << m.second.signal << ", type " << (unsigned) m.second.type << std::endl;
    }
}


prefix_ std::pair<senf::emu::WirelessNLController::DFSState::Enum,std::uint32_t> senf::emu::HardwareWLANInterface::dfsState(unsigned freq, unsigned bandwidth)
{
    if (freq == 0 or bandwidth == 0)
        return std::make_pair(WirelessNLController::DFSState::NoDFS, 0);

    if (bandwidth == MHZ_TO_KHZ(20))
        return wnlc_.dfsState(freq - frequencyOffset_);

    if (bandwidth == MHZ_TO_KHZ(40)) {
        /*             | noDFS  | usable    | available | unavailable
         *       noDFS | noDFS  | usable    | available | unavailable
         *      usable |        | usable    | usable    | unavailable
         *   available |        |           | available | unavailable
         * unavailable |        |           |           | unavailable
         */
        auto state1 (wnlc_.dfsState(freq - frequencyOffset_ - MHZ_TO_KHZ(10)));
        auto state2 (wnlc_.dfsState(freq - frequencyOffset_ + MHZ_TO_KHZ(10)));
        if (state1.first == WirelessNLController::DFSState::NoDFS and state2.first == WirelessNLController::DFSState::NoDFS)
            return std::make_pair(WirelessNLController::DFSState::NoDFS, 0);
        if (state1.first == WirelessNLController::DFSState::Unavailable or state2.first == WirelessNLController::DFSState::Unavailable)
            return std::make_pair(WirelessNLController::DFSState::Unavailable, std::min(state1.second, state2.second));
        if (state1.first == WirelessNLController::DFSState::Usable or state2.first == WirelessNLController::DFSState::Usable)
            return std::make_pair(WirelessNLController::DFSState::Usable, std::min(state1.second, state2.second));
        return std::make_pair(WirelessNLController::DFSState::Available, std::min(state1.second, state2.second));
    }

    if (bandwidth == MHZ_TO_KHZ(80)) {
        /*             | noDFS  | usable    | available | unavailable
         *       noDFS | noDFS  | usable    | available | unavailable
         *      usable |        | usable    | usable    | unavailable
         *   available |        |           | available | unavailable
         * unavailable |        |           |           | unavailable
         */
        auto state1 (wnlc_.dfsState(freq - frequencyOffset_ - MHZ_TO_KHZ(30)));
        auto state2 (wnlc_.dfsState(freq - frequencyOffset_ - MHZ_TO_KHZ(10)));
        auto state3 (wnlc_.dfsState(freq - frequencyOffset_ + MHZ_TO_KHZ(10)));
        auto state4 (wnlc_.dfsState(freq - frequencyOffset_ + MHZ_TO_KHZ(30)));

        if (state1.first == WirelessNLController::DFSState::NoDFS and
            state2.first == WirelessNLController::DFSState::NoDFS and
            state3.first == WirelessNLController::DFSState::NoDFS and
            state4.first == WirelessNLController::DFSState::NoDFS)
            return std::make_pair(WirelessNLController::DFSState::NoDFS, 0);
        
        if (state1.first == WirelessNLController::DFSState::Unavailable or
            state2.first == WirelessNLController::DFSState::Unavailable or
            state3.first == WirelessNLController::DFSState::Unavailable or
            state4.first == WirelessNLController::DFSState::Unavailable)
            return std::make_pair(WirelessNLController::DFSState::Unavailable, std::min({state1.second, state2.second, state3.second, state4.second}));
        
        if (state1.first == WirelessNLController::DFSState::Usable or
            state2.first == WirelessNLController::DFSState::Usable or
            state3.first == WirelessNLController::DFSState::Usable or
            state4.first == WirelessNLController::DFSState::Usable)
            return std::make_pair(WirelessNLController::DFSState::Usable, std::min({state1.second, state2.second, state3.second, state4.second}));
        
        return std::make_pair(WirelessNLController::DFSState::Available, std::min({state1.second, state2.second, state3.second, state4.second}));
    }
    
    throw InvalidArgumentException("invalid bandwidth: ") << bandwidth;
}

prefix_ void senf::emu::HardwareWLANInterface::startCAC(unsigned int freq, unsigned int bandwidth)
{
    if (! enabled())
        throw senf::SystemException(
            "startCAC() failed: Interface (" + device() + ") is down ", ENETDOWN);

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
    case MHZ_TO_KHZ(80):
        channelMode = WirelessNLController::ChannelMode::VHT80;
        freq -= MHZ_TO_KHZ(30);
        break;
    default:
        throw InvalidArgumentException("invalid bandwidth: ") << bandwidth;
    }

    // reset device (CAC)
    netctl_.down();
    netctl_.up();

    wnlc_.start_radarDetection(freq-frequencyOffset_, channelMode);
}

prefix_ void senf::emu::HardwareWLANInterface::cacAborted()
{
    try {
        netctl_.down();
    } catch (...) {};
}

prefix_ senf::emu::WirelessNLController::NetlinkEvent<senf::emu::RadarEvent> & senf::emu::HardwareWLANInterface::radarEvent()
{
    return wnlc_.radarEvent;
}

prefix_ senf::emu::WirelessNLController::NetlinkEvent<senf::emu::ScanEvent> & senf::emu::HardwareWLANInterface::scanEvent()
{
    return wnlc_.scanEvent;
}

prefix_ senf::emu::WirelessNLController & senf::emu::HardwareWLANInterface::wirelessNLController()
{
    return wnlc_;
}

prefix_ void senf::emu::HardwareWLANInterface::dumpSurvey(std::ostream & os)
{
    os << "Lastest survey data, sorted by frequency" << std::endl;

    for( auto const & survey : wnlc_.survey()) {
        os << survey.frequency << ": inUse " << survey.inUse << ", noise " << unsigned(survey.noise) << ", channelTime " << survey.channelTime
           << ", channelTimeBusy " << survey.channelTimeBusy << ", channelTimeExtBusy " << survey.channelTimeExtBusy
           << ", channelTimeRx " << survey.channelTimeRx << ", channelTimeTx " << survey.channelTimeTx << std::endl;
    }
}

prefix_ void senf::emu::HardwareWLANInterface::triggerScan(std::ostream & os, std::set<WirelessNLController::frequency_type> const & frequencies)
{
    os << "Triggering new scan on frequencies " << (frequencies.empty() ? "all" : "(" + senf::stringJoin(frequencies, " ") + ")") << std::endl;
    wnlc_.do_trigger_scan(frequencies);
}

prefix_ void senf::emu::HardwareWLANInterface::getScan(std::ostream & os)
{
    os << "Lastest scan data, sorted by bssId" << std::endl;

    for (auto const & sd : wnlc_.getScan()){
        os << sd.bssId << ": tsf " << sd.tsf << ", frequency " << sd.frequency << ", signal " << sd.signal
           << ", beaconIELength " << sd.beaconInformationElementsLength << ", IELength " << sd.informationElementsLength << std::endl;
    }
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
