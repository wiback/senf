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
    \brief WirelessNLController unit tests */

#include "WirelessNLController.hh"

// Custom includes
#include <senf/Scheduler/TimerEvent.hh>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Utils/String.hh>
#include <senf/Utils/algorithm.hh>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
using namespace senf::emu;

namespace {

    std::string getTestIface()
    {
        char const * iface (getenv("SENF_WLAN_TEST_INTERFACE"));
        if (not iface) {
            BOOST_WARN_MESSAGE(false, "Set SENF_WLAN_TEST_INTERFACE to test the senf::emu::WirelessNLController");
            return "";
        }
        return iface;
    }

}

SENF_AUTO_TEST_CASE(wlanNLController)
{
    SENF_RETURN_NO_ROOT_PRIVILEGES("Cannot run all tests as non-root user");

    std::string iface (getTestIface());
    if (iface.empty())
        return;

    WirelessNLController wnlc (iface);

#   define dumpMCS( optional_rates ) ( optional_rates ? senf::stringJoin( *optional_rates, ", ") : "None" )

    std::cout << "Test senf::emu::WirelessNLController with interface " << iface << std::endl
              << "  PhyName: " << wnlc.phyName() << "  PhyIndex: " << wnlc.phyIndex() << std::endl
              << "  Valid bitrates: " << std::endl
              << "    legacy 2.4: " << dumpMCS( wnlc.bitrates().legacy_24) << std::endl
              << "    legacy 5  : " << dumpMCS( wnlc.bitrates().legacy_5) << std::endl
              << "    MCS 2.4   : " << dumpMCS( wnlc.bitrates().mcs_24) << std::endl
              << "    MCS 5     : " << dumpMCS( wnlc.bitrates().mcs_5) << std::endl
              << "  Supported Bands: ";
    if (senf::contains(wnlc.supportedBands(), WirelessNLController::BAND_2GHZ))
        std::cout << "2GHz ";
    if (senf::contains(wnlc.supportedBands(), WirelessNLController::BAND_5GHZ))
        std::cout << "5GHz ";
    std::cout << std::endl
              << "  Supported Frequencies All : " << senf::stringJoin( wnlc.frequencies(), ", ") << std::endl
              << "  Supported Frequencies 2GHz: " << senf::stringJoin( wnlc.frequencies(WirelessNLController::BAND_2GHZ), ", ") << std::endl
              << "  Supported Frequencies 5GHz: " << senf::stringJoin( wnlc.frequencies(WirelessNLController::BAND_5GHZ), ", ") << std::endl
              << "  Current frequency: " << wnlc.frequency() << " KHz" << std::endl
              << "  Coverage Class: " << wnlc.coverageClass() << std::endl
              << "  Type: " << WirelessNLController::IfaceType::str(wnlc.type()) << std::endl
              << "  All interfaces of the phy device: " << senf::stringJoin( wnlc.all_interfaces(), ", ") << std::endl
              << "  HT Capabilities: " << (wnlc.hasHTCapabilities() ? "yes" : "no") << std::endl;
    if (wnlc.hasHTCapabilities()) {
        HTCapabilitiesInfo const & capInfo (wnlc.htCapabilities());
        std::cout << "    Channel width: " << (capInfo.channelWidth == HTCapabilitiesInfo::HT20 ? "HT20" : "HT20/40") << std::endl;
    }
}

SENF_AUTO_TEST_CASE(wlanNLController_reg)
{
    SENF_RETURN_NO_ROOT_PRIVILEGES("Cannot run all tests as non-root user");

    return;

//    SENF_CHECK_NO_THROW( wnlc.add_monInterface( "mon0"));
//    SENF_CHECK_NO_THROW( wnlc.del_interface( "mon0"));

//    wnlc.ibss_join( "test", 5180000, WirelessNLController::ChannelType::NoHT);
//    SENF_CHECK_NO_THROW( wnlc.set_freq( 5200000));
//    SENF_CHECK_NO_THROW( wnlc.ibss_join("TEST", 5180000, senf::MACAddress::None));

//    wnlc.start_radarDetection(5320000, WirelessNLController::ChannelType::NoHT);

//    RegulatoryRule rule;
//    rule.frequencyRangeBegin = 5470000;
//    rule.frequencyRangeEnd   = 5725000;
//    rule.maxBandwidth = 40000;
//    rule.maxEIRP = 3000;
//    rule.flags = 0;//RegulatoryRule::Flags::DFS; //| RegulatoryRule::Flags::NoIR;
//    std::vector<RegulatoryRule> rules;
//    rules.push_back(rule);
//
//    std::string alpha2 ("00");
//    char const * country (getenv("COUNTRY"));
//    if (country) {
//        alpha2 = std::string(country);
//    }
//
//    WirelessNLController wnlc;
//    wnlc.set_regulatory(alpha2, RegulatoryDomain::DFSRegion::Unset, rules);
}

namespace {

    void setRegulatoryRequest()
    {
        WirelessNLController wnlc;

        std::string currentAlpha2 (wnlc.get_regulatory().alpha2Country);
        std::string newAlpha2 = "AA";
        if (currentAlpha2 == newAlpha2)
            newAlpha2 = "AB";

        std::cout << "Current Regulatory Domain: " << currentAlpha2 << std::endl
                  << "Set to " << newAlpha2 << " to trigger an event" << std::endl;

        wnlc.set_regulatory_request(newAlpha2);
    }

    void regulatoryDomainChange(RegulatoryDomainChangeEvent event)
    {
        std::cout << "regulatoryDomainChangeEvent\n";
        senf::scheduler::terminate();
    }

    void runScheduler(senf::ClockService::clock_type timeout)
    {
        senf::scheduler::TimerEvent timeoutEvent (
                "testTimeout", senf::scheduler::terminate, senf::ClockService::now() + timeout);
        senf::scheduler::process();
    }

}

SENF_AUTO_TEST_CASE(wlanNLController_regEvent)
{
    SENF_RETURN_NO_ROOT_PRIVILEGES("Cannot run all tests as non-root user");

    WirelessNLController wnlc;
    wnlc.regulatoryDomainChangeEvent.signal.connect(&regulatoryDomainChange);
    wnlc.regulatoryDomainChangeEvent.enable();

    senf::scheduler::TimerEvent setRegulatoryRequestEvent (
            "setRegulatoryRequest", &setRegulatoryRequest,
            senf::ClockService::now() + senf::ClockService::milliseconds(250));

    runScheduler(senf::ClockService::seconds(1));
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
// compile-command: "scons -u test"
// End:
