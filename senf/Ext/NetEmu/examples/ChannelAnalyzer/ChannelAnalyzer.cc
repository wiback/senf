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
#include <senf/PPI.hh>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Ext/NetEmu/config.hh>
#include <senf/Ext/NetEmu/WLAN/MonitorDataFilter.hh>
#include <senf/Socket/Protocols/Raw/MMapPacketSocketHandle.hh>
#include <senf/PPI/QueueSocketSourceSink.hh>

#include "FrameAnalyzer.hh"

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

int main(int argc, char const * argv[])
{
    senf::log::ConsoleTarget::logToStderr();

    Configuration configuration;

    if (!configuration.parse( argc, argv)) {
        exit(1);
    }

    senf::TCPv4ClientSocketHandle *handle = NULL;
    if (configuration.logDestination.port() != 0) {
        try {
            handle = new senf::TCPv4ClientSocketHandle( configuration.logDestination);
            handle->protocol().sndbuf( 128000);  // avoid blocking on temporary TCP issues
            if (dup2( handle->fd(), fileno(stdout)) == -1) {
                throw senf::SystemException( "dup2 failed");
            }
        }
        catch (senf::SystemException & ex) {
            SENF_LOG( (senf::log::CRITICAL) ("Failed to connect to log destination: " << ex.message()) );
            exit(1);
        }
    }

    // determine the MAC address of the monitor Device.
    // This is required for the MonitorDataFilter to perform proper re-ordering
    senf::MACAddress mac;
    try {
        std::ifstream fs;
        std::string buf;
        fs.open( (std::string("/sys/class/net/") + configuration.device + std::string("/address")).c_str());
        fs >> buf;
        fs.close();
        mac = senf::MACAddress::from_string(buf);
    } catch(...) {
        SENF_LOG( (senf::log::CRITICAL) ("Failed to determine MAC address of monitor device") );
        exit(1);
    }
    SENF_LOG((senf::log::IMPORTANT)("Determined MAC address for " << configuration.device << " is " << mac));

    senf::ConnectedMMapReadPacketSocketHandle socket(configuration.device, 1024, SENF_EMU_MAXMTU);
    socket.protocol().rcvbuf(4096);

    senf::emu::MonitorDataFilter filter (mac);

    /*
    if (configuration.reorderBufferSize > senf::ClockService::milliseconds(0))
        filter.reorderTimeout(configuration.reorderTimeout);
    */

    if (configuration.tsftHistogram) {
        filter.tsftHistogram().start();
    }

    FrameAnalyzer analyzer (configuration);

    // monitor mode specific config
    filter.promisc(configuration.promisc);
    filter.annotate(true);
    filter.airTime(true);
    senf::emu::MonitorDataFilter::filterMonitorTxFrames(socket);
    // now, create the RT source
    auto *source (new senf::ppi::module::ActiveQueueSocketSource<senf::RadiotapPacket>(socket));
    senf::ppi::connect( *source, filter);
    senf::ppi::connect( filter, analyzer);
    
    senf::scheduler::SignalEvent sigint  (SIGINT,  handleSignal);
    senf::scheduler::SignalEvent sigterm (SIGTERM, handleSignal);

    SENF_LOG((senf::log::IMPORTANT)("Waiting for first valid frame to start..."));

    senf::ppi::run();

    unsigned dropped (socket.protocol().rxQueueDropped());
    if (dropped > 0) {
        SENF_LOG((senf::log::IMPORTANT)("Dropped " << dropped << " frames during the capture! CPU overload ?"));
    }
    SENF_LOG((senf::log::IMPORTANT) ("Filterstats " << filter.stats().dump()) );

    if (configuration.tsftHistogram) {
        filter.tsftHistogram().dump(std::cout);
    }

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
