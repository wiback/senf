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
                  << "  -n  --nloops=<loops over frequency range>\n"
                  << "  -w, --wait=<wait time in sec per frequency>\n"
                  << "  -t, --time=<total time in sec (0 for endless)>\n"
                  << "  -f, --frequencies=<frequencies to scan>\n"
                  << "      --htmode=<NoHT|HT20|HT40Minus|HT40Plus>\n" << std::endl;
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
                << "000 type: ";
            switch (event.type) {
            case senf::emu::RadarEvent::RadarDetected:
                log << "Radar Detected";
                break;
            case senf::emu::RadarEvent::CACFinished:
                log << "CAC Finished";
                senf::scheduler::terminate();
                break;
            case senf::emu::RadarEvent::CACAborted:
                log << "CAC Aborted";
                senf::scheduler::terminate();
                break;
            case senf::emu::RadarEvent::RadarNopFinished:
                log << "Radar-Nop Finished";
                break;
            default:
                log << "Unknown: " << int(event.type);
                break;
            }
        }) );
    }
}

namespace senf { namespace emu {

    SENF_CONSOLE_REGISTER_ENUM_MEMBER(
            WirelessNLController::ChannelType, Enum, (NoHT)(HT20)(HT40Minus)(HT40Plus) );
}}

namespace emu = senf::emu;

int run(int argc, char const * argv[])
{
    std::string iface;
    unsigned nloops = 100000000;
    unsigned waitTime = 60;
    unsigned totalTime = 0;
    senf::console::ValueRange<unsigned> frequencyRange = { 0, 0 };
    emu::WirelessNLController::ChannelType::Enum htMode = emu::WirelessNLController::ChannelType::NoHT;

    namespace fty = senf::console::factory;
    senf::console::root().add("help",        fty::Command(&print_usage_and_exit));
    senf::console::root().add("interface",   fty::Variable(iface));
    senf::console::root().add("nloops",      fty::Variable(nloops));
    senf::console::root().add("wait",        fty::Variable(waitTime));
    senf::console::root().add("time",        fty::Variable(totalTime));
    senf::console::root().add("frequencies", fty::Variable(frequencyRange));
    senf::console::root().add("htmode",      fty::Variable(htMode));
    senf::console::root().add("crda",        emu::CRDA::instance().dir);

    std::vector<std::string> nonOptions;
    senf::console::ProgramOptions cmdlineOptions (argc, argv);
    cmdlineOptions
        .alias('i', "--interface", true)
        .alias('n', "--nloops", true)
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

    // only allow DFS channels
    emu::RegulatoryDomain dfsRegDomain_;
    dfsRegDomain_.alpha2Country = "XX";
    dfsRegDomain_.dfsRegion = emu::RegulatoryDomain::DFSRegion::ETSI;
    dfsRegDomain_.rules.insert(emu::RegulatoryRule()
                               .frequencyRange(5250000, 5330000)
                               .maxBandwidth(40000)
                               .maxEIRP(3000)
                               .noIR(true)
                               .dfsRequired(true) );
    dfsRegDomain_.rules.insert(emu::RegulatoryRule()
                               .frequencyRange(5490000, 5730000)
                               .maxBandwidth(40000)
                               .maxEIRP(3000)
                               .noIR(true)
                               .dfsRequired(true) );

    if (not emu::CRDA::instance().regDomain().isEqual(dfsRegDomain_)) {
        std::cout << "Settings DFS-only regDomain...";
        if (not emu::CRDA::instance().regDomain(dfsRegDomain_)) {
            std::cout << "Failed !" << std::endl;
            return 1;
        } else {
            std::cout << "OK" << std::endl;
        }

        sleep(2);

        if (emu::CRDA::instance().equalsKernel()) {
            std::cout << "Current regDomain updated to " << emu::CRDA::instance().regDomain() << std::endl;
        } else {
            std::cout << "Requested regDomain does not match the kernel's regDomain !!!" << std::endl;
            return 1;
        }
    } else {
        std::cout << "Current regDomain is already " << emu::CRDA::instance().regDomain() << std::endl;
    }

    emu::WirelessNLController wnlc (iface);

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
        if (wnlc.dfsState(*f) == emu::WirelessNLController::DFSState::NoDFS) {
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
        if (not emu::WirelessExtController(iface).ssid().empty())
            wnlc.ibss_leave();
    }

    wnlc.radarEvent.signal.connect(&handleRadarEvent);
    wnlc.radarEvent.enable();

    if (totalTime)
        senf::scheduler::TimerEvent timeoutEvent (
                "dfstest total timeout", boost::bind(::exit, 0),
                senf::ClockService::now() + senf::ClockService::seconds(totalTime));

    while (nloops-- > 0) {
        for (unsigned freq : dfsFreqs) {
            switch (wnlc.dfsState(freq)) {
            case emu::WirelessNLController::DFSState::Usable:
                SENF_LOG( ("Performing CAC for frequency " << freq << " (" << senf_console_format_enum(htMode) << ")") );
                wnlc.start_radarDetection(freq, htMode);
                runScheduler(senf::ClockService::seconds(61));
                continue;
            case emu::WirelessNLController::DFSState::Unavailable:
                SENF_LOG( ("DFS state for frequency " << freq << " is Unavailable; skipping frequency.") );
                continue;
            case emu::WirelessNLController::DFSState::Available:
                SENF_LOG( ("Join cell on frequency " << freq << " (" << senf_console_format_enum(htMode)
                        << ") and wait " << waitTime << " seconds for radar events.") );
                wnlc.ibss_join("DFS_test", freq, htMode)
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
    if (boost::filesystem::path(argv[0]).filename() == emu::CRDA::instance().slaveName()) {
        emu::CRDA::instance().init();
        return emu::CRDA::instance().run(argc, argv);
    }

    // true ==> MasterMode
    if (!emu::CRDA::instance().init(true)) {
        std::cerr << "Failed to init CRDA ?!?" << std::endl;
        return 2;
    }

    try {
        return run( argc, argv);
    } catch (senf::Exception & e) {
        std::cerr << e.message() << std::endl;
        return 2;
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
