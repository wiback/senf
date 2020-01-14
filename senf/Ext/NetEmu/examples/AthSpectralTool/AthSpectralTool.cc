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


#include <fstream>

// Custom includes
#include <boost/format.hpp>
#include <senf/PPI.hh>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Ext/NetEmu/config.hh>
#include <senf/Socket/NetdeviceController.hh>
#include <senf/Ext/NetEmu/WLAN/WirelessNLController.hh>
#include <senf/Socket/Protocols/Raw/MMapPacketSocketHandle.hh>
#include <senf/PPI/QueueSocketSourceSink.hh>

#include "Analyzer.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

void handleSignal(siginfo_t const &)
{
    senf::scheduler::terminate();
}

namespace {
    int exitCode_ (0);
}

void exitCode(int code)
{
    exitCode_ = code;
}

prefix_ std::string initInterface(Configuration & config)
{
    senf::emu::WirelessNLController wnlc (senf::emu::WirelessNLController(config.phyName).phyName());

    // first, remove all existing interfaces of the phy device
    for (std::string const & iface : wnlc.all_interfaces()) {
        {
            senf::NetdeviceController netdevCtrl (iface);
            netdevCtrl.down();
        }
        wnlc.del_interface(iface);
    }

    std::string name ((boost::format("phy%d-mon") % wnlc.phyIndex()).str());

    wnlc.add_monInterface(name,
            senf::emu::WirelessNLController::MonitorFlags::FCSFail  |
            senf::emu::WirelessNLController::MonitorFlags::PLCPFail |
            senf::emu::WirelessNLController::MonitorFlags::Control  |
            senf::emu::WirelessNLController::MonitorFlags::OtherBSS);
    
    senf::NetdeviceController(name).up();

    wnlc.set_frequency(config.frequency*1000, config.ht40 ? senf::emu::WirelessNLController::ChannelMode::HT40Plus : senf::emu::WirelessNLController::ChannelMode::HT20);
    
    return name;
}

int main(int argc, char const * argv[])
{
    senf::log::ConsoleTarget::logToStderr();

    Configuration configuration;

    if (!configuration.parse( argc, argv)) {
        exit(1);
    }

    std::string ifName (initInterface(configuration));

    senf::ConnectedMMapReadPacketSocketHandle socket(ifName, 1024, SENF_EMU_MAXMTU);
    senf::ppi::module::ActiveQueueSocketSource<senf::RadiotapPacket> source(socket);
    socket.protocol().rcvbuf(SENF_EMU_MAXMTU);

    Analyzer analyzer (configuration);
    
    senf::ppi::connect( source, analyzer);

    senf::scheduler::SignalEvent sigint  (SIGINT,  handleSignal);
    senf::scheduler::SignalEvent sigterm (SIGTERM, handleSignal);

    if (!analyzer.startSpectralScan()) {
        SENF_LOG((senf::log::IMPORTANT)("Failed to setup/start the spectral scanner") );
        exit(1);
    }

    SENF_LOG((senf::log::IMPORTANT)("Specral settings"
                                    << ": period (atk9k)" << analyzer.athSpectralScan().spectralPeriod()
                                    << ", fft-period (ath9k)" << analyzer.athSpectralScan().spectralFFTPeriod()
                                    << ", short-repeat (ath9k)" << analyzer.athSpectralScan().spectralShortRepeat()
                                    << ", bins (ath10k)" << analyzer.athSpectralScan().spectralBins()
                                    << ", count " << analyzer.athSpectralScan().spectralCount() ) );

    senf::ppi::run();

    unsigned dropped (socket.protocol().rxQueueDropped());
    if (dropped > 0) {
        SENF_LOG((senf::log::IMPORTANT)("Dropped " << dropped << " frames during the capture! CPU overload ?"));
    }

    SENF_LOG((senf::log::MESSAGE) (analyzer.stats()) );
    
    return exitCode_;
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u"
// comment-column: 40
// End:
