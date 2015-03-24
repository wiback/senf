//
// Copyright (C) 2013
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//   Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de
//

// Custom includes
#include <senf/PPI.hh>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Socket/Protocols/Raw.hh>
#include <senf/Ext/NetEmu/WLAN/MonitorDataFilter.hh>
#include <senf/Ext/NetEmu/WLAN/MmapSocketSourceRadioTap.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

void handleSignal(siginfo_t const &)
{
    senf::scheduler::terminate();
}

void print_usage_and_exit()
{
    std::cerr << "usage: wlanMon2Eth <monitor_iface> <tap_iface> <reorder|no-reorder>" << std::endl;
    exit(1);
}

int main(int argc, char const * argv[])
{
    if (argc != 4) {
        print_usage_and_exit();
    }
    if (getuid() != 0) {
        std::cerr << "you must be root!" << std::endl;
        print_usage_and_exit();
    }

    senf::scheduler::SignalEvent sigint  (SIGINT,  handleSignal);
    senf::scheduler::SignalEvent sigterm (SIGTERM, handleSignal);

    senf::PacketSocketHandle srcSocket;
    srcSocket.bind(senf::LLSocketAddress(argv[1]));
    srcSocket.blocking(false);
    senf::MmapSocketSourceRadioTap source (srcSocket, 1024);

    senf::TapSocketHandle dstSocket (argv[2]);
    senf::ppi::module::PassiveSocketSink<> sink (dstSocket);

    senf::emu::MonitorDataFilter filter;
    filter.promisc(true);
    if (std::string(argv[3]) == "no-reorder") {
        filter.reorderPackets(false);
    } else {
        if (std::string(argv[3]) == "reorder") {
            filter.reorderPackets(true);
        } else {
            print_usage_and_exit();
        }
    }

    senf::ppi::connect(source, filter);
    senf::ppi::connect(filter, sink);
    senf::ppi::run();

    return 0;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
