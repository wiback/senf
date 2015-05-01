//
// Copyright (C) 2014
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>
//

// Custom includes
#include <boost/filesystem.hpp>
#include <senf/Scheduler/TimerEvent.hh>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Socket/NetdeviceController.hh>
#include <senf/Utils/String.hh>
#include <senf/Utils/algorithm.hh>
#include <senf/Utils/Logger.hh>
#include <senf/Ext/NetEmu/WLAN/WirelessExtController.hh>
#include <senf/Ext/NetEmu/WLAN/WirelessNLController.hh>
#include <senf/Ext/NetEmu/WLAN/CRDA.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace {

    void print_usage_and_exit()
    {
        std::cerr << "usage: dfstest \n"
                  << "  -i, --interface=<adhoc_iface>\n"
                  << "  -w, --wait=<wait time in sec per frequency>\n"
                  << "  -t, --time=<total time in sec (0 for endless)>\n"
                  << "  -f, --frequencies=<frequencies to scan>\n" << std::endl;
        exit(1);
    }

    void runScheduler(senf::ClockService::clock_type timeout)
    {
        senf::scheduler::TimerEvent timeoutEvent (
                "dfstest timeout", senf::scheduler::terminate, senf::ClockService::now() + timeout);
        senf::scheduler::process();
    }

    void handleRadarEvent(senf::emu::RadarEvent event)
    {
        SENF_LOG_BLOCK( ({
            log << "RadarEvent received for frequency " << event.frequency
                << " type: ";
            switch (event.type) {
            case senf::emu::RadarEvent::RadarDetected:
                log << "Radar Detected "  << event.frequency;
                break;
            case senf::emu::RadarEvent::CACFinished:
                log << "CAC Finished" << event.frequency;
                break;
            case senf::emu::RadarEvent::CACAborted:
                log << "CAC Aborted" << event.frequency;
                break;
            case senf::emu::RadarEvent::RadarNopFinished:
                log << "Radar-Nop Finished" << event.frequency;
                break;
            default:
                log << "Unknown: " << int(event.type) << " " << event.frequency;
                break;
            }
        }) );
    }
}

//int run(int argc, char const * argv[]);

int run(int argc, char const * argv[])
{
    std::string iface;
    unsigned waitTime = 60;
    unsigned totalTime = 0;
    senf::console::ValueRange<unsigned> frequencyRange = { 0, 0 };

    namespace fty = senf::console::factory;
    senf::console::root().add("help",        fty::Command(&print_usage_and_exit));
    senf::console::root().add("interface",   fty::Variable(iface));
    senf::console::root().add("wait",        fty::Variable(waitTime));
    senf::console::root().add("time",        fty::Variable(totalTime));
    senf::console::root().add("frequencies", fty::Variable(frequencyRange));
    senf::console::root().add("crda",        senf::emu::CRDA::instance().dir);

    std::vector<std::string> nonOptions;
    senf::console::ProgramOptions cmdlineOptions (argc, argv);
    cmdlineOptions
        .alias('i', "--interface", true)
        .alias('w', "--wait", true)
        .alias('t', "--time", true)
        .alias('f', "--frequencies", true)
        .nonOptions(nonOptions);
    try {
        cmdlineOptions.parse();
    } catch (senf::console::SyntaxErrorException const & e) {
        std::cerr << e.message() << std::endl;
        print_usage_and_exit();
    }
    if (not nonOptions.empty())
        print_usage_and_exit();

    if (iface.empty())
        print_usage_and_exit();


    std::cout << "Current regDomain: "; 
    senf::console::format(senf::emu::CRDA::instance().regulatoryDomain(), std::cout);
    std::cout << std::endl;

    // only allow DFS channels
    senf::emu::RegulatoryDomain dfsRegDomain_;
    dfsRegDomain_.alpha2Country = "AA";
    dfsRegDomain_.dfsRegion = senf::emu::RegulatoryDomain::DFSRegion::ETSI;
    dfsRegDomain_.rules.push_back(senf::emu::RegulatoryRule()
                                  .frequencyRange(5250000, 5330000)
                                  .maxBandwidth(40000)
                                  .maxEIRP(3000)
                                  .noIR(true)
                                  .dfsRequired(true) );
    dfsRegDomain_.rules.push_back(senf::emu::RegulatoryRule()
                                  .frequencyRange(5490000, 5730000)
                                  .maxBandwidth(40000)
                                  .maxEIRP(3000)
                                  .noIR(true)
                                  .dfsRequired(true) );
    
    std::cout << "Settings DFS-only regDomain...";
    senf::emu::CRDA::instance().regulatoryDomain(dfsRegDomain_);
    if (!senf::emu::CRDA::instance().setNextDummyRegCountry()) {
        std::cout << "Failed !" << std::endl;
        return 1;
    } else {
        std::cout << "OK" << std::endl;
    }

    sleep(2);

    std::cout << "Current regDomain: "; 
    senf::console::format(senf::emu::CRDA::instance().regulatoryDomain(), std::cout);
    std::cout << std::endl;

    senf::emu::WirelessNLController wnlc (iface);

    auto frequencies (wnlc.frequencies());
    std::set<unsigned> dfsFreqs (frequencies.begin(), frequencies.end());
    for (auto f = dfsFreqs.begin(); f != dfsFreqs.end();) {
        if (*f < frequencyRange.lower) {
            dfsFreqs.erase(f++);
            continue;
        }
        if (frequencyRange.upper != 0 and *f > frequencyRange.upper) {
            dfsFreqs.erase(f++);
            continue;
        }
        if (wnlc.dfsState(*f) == senf::emu::WirelessNLController::DFSState::NoDFS) {
            dfsFreqs.erase(f++);
            continue;
        }
        ++f;
    }

    if (dfsFreqs.empty()) {
        std::cout << "No DFS-required frequencies found; stopping test.";
        return 1;
    }

    auto & logTarget (senf::log::ConsoleTarget::instance());
    logTarget.route<senf::log::MESSAGE>();
    logTarget.tag("");
    logTarget.showLevel(false);
    logTarget.timeFormat("%Y-%m-%d %H:%M:%S");

    SENF_LOG( ("Running DFS Test on interface " << iface) );
    SENF_LOG( ("Using Interfaces " << senf::stringJoin(dfsFreqs, ", ")) );

    {
        senf::NetdeviceController ctrl (iface);
        if (not ctrl.isUp())
            ctrl.up();
        if (not senf::emu::WirelessExtController(iface).ssid().empty())
            wnlc.ibss_leave();
    }

    wnlc.radarEvent.signal.connect(&handleRadarEvent);
    wnlc.radarEvent.enable();

    if (totalTime)
        senf::scheduler::TimerEvent timeoutEvent (
                "dfstest total timeout", boost::bind(::exit, 0),
                senf::ClockService::now() + senf::ClockService::seconds(totalTime));

    while (true) {
        for (unsigned freq : dfsFreqs) {
            switch (wnlc.dfsState(freq)) {
            case senf::emu::WirelessNLController::DFSState::Usable:
                SENF_LOG( ("Performing CAC for frequency " << freq) );
                wnlc.start_radarDetection(freq);
                runScheduler(senf::ClockService::seconds(61));
                continue;
            case senf::emu::WirelessNLController::DFSState::Unavailable:
                SENF_LOG( ("DFS state for frequency " << freq << " is Unavailable; skipping frequency.") );
                continue;
            case senf::emu::WirelessNLController::DFSState::Available:
                SENF_LOG( ("Join cell on frequency " << freq << " and wait " << waitTime << " seconds for radar events.") );
                wnlc.ibss_join("DFS_test", freq, senf::emu::WirelessNLController::ChannelType::NoHT)
                        ->handleDFS(true);
                runScheduler(senf::ClockService::seconds(waitTime));
                break;
            default:
                SENF_LOG( ("Unknown DFS state: " << int(wnlc.dfsState(freq))) );
                return 1;
            }
            wnlc.ibss_leave();
        }
    }

    return 0;
}

int main(int argc, char const * argv[])
{
    if (boost::filesystem::path(argv[0]).filename() == "wiback-crda")
        return senf::emu::CRDA::instance().run(argc, argv);

    try {
        return run( argc, argv);
    } catch (senf::Exception & e) {
        std::cerr << e.message() << std::endl;
        return 2;
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
