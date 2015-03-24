// $Id: main.cc 824 2013-10-24 09:28:10Z tho $
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
//

/** \file
    \brief main non-inline non-template implementation */

//#include "main.hh"
//#include "main.ih"

// Custom includes
#include <boost/range/iterator_range.hpp>
#include <boost/bind.hpp>
#include <senf/PPI.hh>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Utils/Logger.hh>
#include <senf/Utils/Console.hh>
#include <senf/Utils/StatisticsTargets.hh>
#include <senf/Ext/NetEmu/WLAN/EmulatedWLANInterface.hh>
#include <senf/Ext/NetEmu/WLAN/HardwareWLANInterface.hh>
#include <senf/Ext/NetEmu/Ethernet/HardwareEthernetInterface.hh>
#include "ApplicationStarter.hh"
#include "Config.hh"
#include "Setup.hh"
#include "Dummy.hh"
#include "Decoration.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    void sigint(siginfo_t const &)
    {
        senf::scheduler::terminate();
    }

    void addConfigFile(senf::console::ConfigBundle & bundle, std::string const & name)
    {
        bundle.add(senf::console::FileConfig(name));
    }
}

int main(int argc, char const * argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: carmen [--help] [<options>...]" << std::endl;
        return 1;
    }

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // Configuration

    namespace fty = senf::console::factory;

    wiback::Config config;
    typedef wiback::Setup<wiback::CARMENDecoration> Setup;
    Setup setup;

    setup.registerInterfaceType<senf::emu::EmulatedWLANInterface>(
        "emulatedWLANInterface");
    setup.registerInterfaceType<senf::emu::HardwareWLANInterface, std::string>(
        "hardwareWLANInterface", "dev", "device name");
    setup.registerInterfaceType<senf::emu::HardwareEthernetInterface, std::string>(
        "hardwareEthernetInterface", "dev", "device name");

    senf::console::ConfigBundle bundle (config.dir);

    senf::console::root().add("conf", config.dir);
    senf::console::root().add("interfaces", fty::Link(senf::emu::interfaceDir()));
    config.dir.add("interfaces", fty::Link(senf::emu::interfaceDir()));
    config.dir.add("log", fty::Link(senf::console::root()["sys"]["log"]));
    config.dir.node().add("config",
                          fty::Command<void (std::string const &)>(
                              boost::bind(&addConfigFile, boost::ref(bundle), _1)));

    bundle.add(senf::console::OptionsConfig(argc, argv));

    bundle.parse();

    if (setup.interfaces().size() == 0) {
        std::cerr << "No interface configured" << std::endl;
        return 1;
    }

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // Logging setup

    senf::log::timeSource<senf::scheduler::LogTimeSource>();

    senf::log::ConsoleTarget & console (senf::log::ConsoleTarget::instance());
    console.showLevel(false);
    console.timeFormat("");
    console.tag("node-" + senf::str(senf::emu::Interface::nodeId()));

    if (config.silent())
        console.route<senf::log::CRITICAL>();
    else if (config.quiet())
        console.route<senf::log::IMPORTANT>();
    else
        console.route<senf::log::VERBOSE>();

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // Optionally start interactive console

    if (config.consolePort() != 0u)
        senf::console::Server::start(
            senf::INet4SocketAddress(senf::INet4Address::Loopback, config.consolePort()));

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // Setup and run CARMEN app

    setup.init(config);
    wiback::DummyRouter router (config, setup);
    wiback::ApplicationStarter starter (senf::ClockService::seconds(2),
                                        senf::membind(&wiback::DummyRouter::start, &router));

    senf::ppi::connect(setup, router);
    senf::ppi::connect(router, setup);

    {
        senf::scheduler::SignalEvent sigintEvent (SIGINT, &sigint);
        senf::ppi::run();
    }

    //-/////////////////////////////////////////////////////////////////////////////////////////////
    // Output summary statistics

    console.route();
    Setup::InterfaceRange ifaces (setup.interfaces());
    for (Setup::InterfaceRange::iterator i (ifaces.begin()); i != ifaces.end(); ++i)
        SENF_LOG_BLOCK(({
            log << "Interface " << i->interface().id() << "\n";
            if (i->canTransmit())
                log << "  packets sent: "
                    << wiback::CARMENDecoration::transmitModule(*i).sentPackets() << "\n";
            if (i->canReceive())
                log << "  packets received: "
                    << wiback::CARMENDecoration::receiveModule(*i).receivedPackets() << "\n";
            senf::emu::EmulatedReceiver const * emu (
                dynamic_cast<senf::emu::EmulatedReceiver const *>(&i->interface()));
            if (emu)
                log <<
                    "  dropped packets: " << emu->droppedPackets() << "\n"
                    "  delayed packets: " << emu->delayedPackets() << "\n"
                    "  max packet delay: "<< senf::ClockService::reltime(emu->maxPacketDelay());
        }));
    SENF_LOG(("delayed timer ticks: " << router.delayedTicks() << "\n"
              "max timer tick delay: " << senf::ClockService::reltime(router.maxTickDelay())));
    return 0;
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
// compile-command: "scons -U"
// End:
