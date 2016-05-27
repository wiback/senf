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
//     Mathias Kretschmer <mathias.kretschmer@fit.fraunhofer.de>


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

    wnlc.set_frequency(config.frequency*1000, config.ht40 ? senf::emu::WirelessNLController::ChannelType::HT40Plus : senf::emu::WirelessNLController::ChannelType::HT20);
    
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

    analyzer.startSpectralScan();
    
    senf::ppi::run();

    unsigned dropped (socket.protocol().rxQueueDropped());
    if (dropped > 0) {
        SENF_LOG((senf::log::IMPORTANT)("Dropped " << dropped << " frames during the capture! CPU overload ?"));
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
