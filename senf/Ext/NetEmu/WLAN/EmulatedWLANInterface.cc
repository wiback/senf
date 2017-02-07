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

/** \file
    \brief EmulatedWLANInterface non-inline non-template implementation */

#include "EmulatedWLANInterface.hh"

// Custom includes
#include <boost/foreach.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <senf/Utils/Console/STLSupport.hh>
#include <senf/Packets/80211Bundle/WLANBeaconPacket.hh>
#include <senf/Packets/80211Bundle/MCSInfo.hh>
#include <senf/Ext/NetEmu/AnnotationsPacket.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::EmulatedWLANInterfaceNet

prefix_ senf::emu::detail::EmulatedWLANInterfaceNet::EmulatedWLANInterfaceNet()
    : receiveFilter_(self()), transmitFilter_(self()),
      beaconGenerator_(self()),
      receiveInput(receiveFilter_.input),
      receiveOutput(receiveFilter_.output), transmitInput (transmitFilter_.input),
      transmitOutput(transmitFilter_.output)
{}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::EmulatedWLANInterface

namespace {
    // Arbitrary magic number used to identify the emulation header for WLAN packets
    // Defined to be the 32bit integer representation of the string 'WLAN' in network byte order
    boost::uint32_t const wlanMagic (0x574c414eu);
}

prefix_ senf::emu::EmulatedWLANInterface::EmulatedWLANInterface()
    : Base(receiveOutput, transmitInput), modulationId_(0),
      power_(0), mode_(ADHOC)
{
    ppi::connect(receiverJack, receiveInput);
    ppi::connect(transmitOutput, transmitterJack);
    ppi::connect(beaconGenerator_, transmitFilter_.beaconInput);

    disable();
    id( uniqueMAC(wlanMagic));
    mtu(1500u);

    registerModulation( WLANModulationParameterRegistry::instance().parameterIdUnknown());
    registerModulation( WLANModulationParameterRegistry::instance().parameterIdAuto());
    init();
}

namespace senf { namespace emu {

    SENF_CONSOLE_REGISTER_ENUM_MEMBER(
        EmulatedWLANInterface, Mode, (ADHOC)(STA)(AP) );
}}

prefix_ void senf::emu::EmulatedWLANInterface::init()
{
    Base::init();

    namespace kw = console::kw;
    namespace fty = console::factory;
    consoleDir().add("beaconInterval", fty::Command(
            SENF_MEMBINDFNP(void, EmulatedWLANInterface, beaconInterval, (ClockService::clock_type)))
        .arg("interval", "Beacon interval for emulated interfaces",
                kw::parser=senf::parseClockServiceInterval)
        .doc("Get/set the Beacon interval.\n"
                "An interval of 0 disables the sending of beacons") );
    consoleDir().add("beaconInterval", fty::Command(
            SENF_MEMBINDFNP(ClockService::clock_type, EmulatedWLANInterface, beaconInterval, () ))
        .formatter(senf::formatClockServiceInterval) );
    consoleDir().add("mode", fty::Command(
            SENF_MEMBINDFNP(void, EmulatedWLANInterface, mode, (EmulatedWLANInterface::Mode) ))
         .arg("mode", "ADHOC, STA or AP")
         .doc("Get/set the mode of a WLAN interface") );
    consoleDir().add("mode", fty::Command(
            SENF_MEMBINDFNP(EmulatedWLANInterface::Mode, EmulatedWLANInterface, mode, () )) );
    consoleDir().add("registerModulation", fty::Command(
            SENF_MEMBINDFNP(void, EmulatedWLANInterface, registerModulation, (WLANModulationParameter::Type, std::vector<std::string>) ))
        .arg("type", "'mcs' or 'legacy'")
        .arg("args", "if type is mcs, args can be \n  'all' for all mcs indexes,\n"
                "  a range like '1-10' for all indexes in that range, or\n"
                "  a list of indexes like '(1 4 6 10)'\n"
                "if type is legacy, args can be \n  'all' for all legacy rates, or\n"
                "  a list of rates like '(360, 540)")
        .doc("register modulations") );
}

prefix_ void senf::emu::EmulatedWLANInterface::v_id(MACAddress const & eui)
{
    id_ = eui;
}

prefix_ senf::MACAddress senf::emu::EmulatedWLANInterface::v_id()
    const
{
    return id_;
}

prefix_ senf::emu::ModulationParameter::id_t senf::emu::EmulatedWLANInterface::v_modulationId()
    const
{
    return modulationId_;
}

prefix_ void senf::emu::EmulatedWLANInterface::v_modulationId(ModulationParameter::id_t id)
{
    WLANModulationParameterRegistry::instance().findModulationById( id);
    modulationId_ = id;
}

prefix_ int senf::emu::EmulatedWLANInterface::v_txPower()
    const
{
    return power_;
}

prefix_ void senf::emu::EmulatedWLANInterface::v_txPower(int power)
{
    power_ = power;
}

prefix_ void senf::emu::EmulatedWLANInterface::registerModulation(ModulationParameter::id_t id)
{
    WirelessInterface::registerModulation( id);
    if (modulationId_ == 0)
        modulationId_ = id;
}


prefix_ void senf::emu::EmulatedWLANInterface::registerVHTModulation(unsigned vhtMcsIndex, unsigned streams, unsigned bandwidth, bool shortGI)
{
    WLANModulationParameterRegistry const & registry (WLANModulationParameterRegistry::instance());
    registerModulation(registry.parameterIdByMCS_VHT(vhtMcsIndex, streams, bandwidth, shortGI));
}

prefix_ void senf::emu::EmulatedWLANInterface::registerHTModulation(unsigned mcsIndex)
{
    WLANModulationParameterRegistry const & registry (WLANModulationParameterRegistry::instance());
    registerModulation(registry.parameterIdByMCS_HT(mcsIndex, false, false));
    registerModulation(registry.parameterIdByMCS_HT(mcsIndex, true,  false));
    registerModulation(registry.parameterIdByMCS_HT(mcsIndex, false, true ));
    registerModulation(registry.parameterIdByMCS_HT(mcsIndex, true,  true ));
}

prefix_ void senf::emu::EmulatedWLANInterface::registerLegacyModulation(unsigned rate)
{
    registerModulation( WLANModulationParameterRegistry::instance().parameterIdByLegacyRate(rate));
}

prefix_ void senf::emu::EmulatedWLANInterface::registerModulation(WLANModulationParameter::Type type, std::vector<std::string> args)
{
#define catch_bad_lexical_cast(arg)                                                     \
        catch (boost::bad_lexical_cast &) {                                             \
            throw console::SyntaxErrorException("argument syntax error: ") << arg;      \
        }

    switch (type) {
    case WLANModulationParameter::VHT:
        if (args.size() != 1 or not boost::algorithm::iequals(args[0], "all"))
            throw console::SyntaxErrorException("vht argument must be 'all'");
        BOOST_FOREACH( senf::WLAN_MCSInfo::Info const & mcsInfo, senf::WLAN_MCSInfo::getInfos() ) {
            if (mcsInfo.index >= WLAN_MCSInfo::NUM_VHT_INDEX)
               continue;
            for (unsigned i=0; i<8; ++i) {
               if (mcsInfo.rate[i] == 0)
                   continue;
               registerVHTModulation(mcsInfo.index, mcsInfo.streams, WLAN_MCSInfo::fromBandwidthIndex(i), (i%2==1));
            }
        }
        return;
    case WLANModulationParameter::HT: {
        if (args.size() == 1) {
            if (boost::algorithm::iequals(args[0], "all")) {  // register all HT modulations
                BOOST_FOREACH( senf::WLAN_MCSInfo::Info const & mcsInfo, senf::WLAN_MCSInfo::getInfos() ) {
                    if (mcsInfo.index >= WLAN_MCSInfo::NUM_HT_INDEX)
                        continue;
                    registerHTModulation(WLAN_MCSInfo::toHTIndex(mcsInfo.index, mcsInfo.streams));
                }
                return;
            }
            std::string::size_type p = args[0].find('-');
            if (p != std::string::npos) {
                try {   // register all modulations in range
                    unsigned start (boost::lexical_cast<unsigned>(args[0].substr(0, p)));
                    unsigned end (boost::lexical_cast<unsigned>(args[0].substr(p+1)));
                    for (unsigned i=start; i<=end; ++i)
                        registerHTModulation(i);
                } catch_bad_lexical_cast(args[0]);
                return;
            }
            try {  // register one modulation index
                registerHTModulation(boost::lexical_cast<unsigned>(args[0]));
            } catch_bad_lexical_cast(args[0]);
            return;
        }
        BOOST_FOREACH( std::string index, args ) {
            try {  // register all given modulation indexes
                registerHTModulation(boost::lexical_cast<unsigned>(index));
            } catch_bad_lexical_cast(index);
        }
        return;
    }
    case WLANModulationParameter::Legacy: {
        if (args.size() == 1 && boost::algorithm::iequals(args[0], "all")) {  // register all legacy modulations
            BOOST_FOREACH( WLANModulationParameterRegistry::LegacyModulationInfo info,
                    WLANModulationParameterRegistry::getLegacyModulationInfos11b() ) {
                registerLegacyModulation( info.rate);
            }
            BOOST_FOREACH( WLANModulationParameterRegistry::LegacyModulationInfo info,
                    WLANModulationParameterRegistry::getLegacyModulationInfosOFDM() ) {
                registerLegacyModulation( info.rate);
            }
            return;
        }
        BOOST_FOREACH( std::string rate, args ) {
            try {  // register all given rates
                registerLegacyModulation( boost::lexical_cast<unsigned>(rate));
            } catch_bad_lexical_cast(rate);
        }
        return;
    }
    case WLANModulationParameter::Automatic:
    case WLANModulationParameter::Unknown:
        break;
    }
    throw console::SyntaxErrorException("argument syntax error: first argument must be 'ht' or 'legacy': ") << type;

#undef catch_bad_lexical_cast
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::EmulatedWLANReceiveFilter

prefix_ senf::emu::detail::EmulatedWLANReceiveFilter::
EmulatedWLANReceiveFilter(EmulatedWLANInterface & iface)
    : iface_ (iface)
{
    route(input, output);
    input.onRequest(&EmulatedWLANReceiveFilter::request);
}

prefix_ void senf::emu::detail::EmulatedWLANReceiveFilter::request()
{

    WLANPacketHeader p;
    try {
        // in case DVB Sender is tuned on the same frequency this
        // will cause an exception
        p = input();
    }
    catch (senf::WrapException<std::bad_cast> &) {
        SENF_LOG( (senf::log::CRITICAL) ("ignoring non-emulated WLAN frame"));
        return;
    }

    EthernetPacket e (p.find<EthernetPacket>(senf::nothrow));
    if (e) {
        if (iface_.annotationMode())
            output(prependAnnotationsPacket(e));
        else
            output(e);
        return;
    }
    WLANPacket_MgtFrame wmp (p.find<WLANPacket_MgtFrame>(senf::nothrow));
    if (wmp) {
        output(prependAnnotationsPacket(wmp));
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::EmulatedWLANTransmitFilter

prefix_ senf::emu::detail::EmulatedWLANTransmitFilter::
EmulatedWLANTransmitFilter(EmulatedWLANInterface & iface)
    : iface_ (iface)
{
    route(input, output);
    route(beaconInput,output);
    input.onRequest(&EmulatedWLANTransmitFilter::request);
    beaconInput.onRequest(&EmulatedWLANTransmitFilter::beaconRequest);
}

prefix_ void senf::emu::detail::EmulatedWLANTransmitFilter::request()
{
    EthernetPacket e (input());
    e->source() = iface_.id();
    WLANPacketHeader wph (WLANPacketHeader::createBefore(e));
    wph.finalizeThis();
    output(wph);
}

prefix_ void senf::emu::detail::EmulatedWLANTransmitFilter::beaconRequest()
{
    WLANPacket_MgtFrame wmp (beaconInput());
    WLANPacketHeader wph (WLANPacketHeader::createBefore(wmp));
    wph->mgt() = true;
    wph.finalizeThis();
    output(wph);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::detail::EmulatedWLANBeaconGenerator
prefix_ senf::emu::detail::EmulatedWLANBeaconGenerator::
EmulatedWLANBeaconGenerator(EmulatedWLANInterface & iface)
#ifdef SENF_DEBUG
    : timer_(ClockService::seconds(2)),
#else
    : timer_(ClockService::milliseconds(250)),
#endif
      iface_(iface)
{
    route(timer_,output);
    init();
}

prefix_ void senf::emu::detail::EmulatedWLANBeaconGenerator::init()
{
    registerEvent( timer_, &EmulatedWLANBeaconGenerator::tick );

    //create beacon packet
    WLANBeaconPacket p (WLANBeaconPacket::create());
    p->beaconInterval() = ClockService::in_milliseconds(timer_.interval().first);
    p->ssidIE().value() << "NetEMU";

    beacon_ = WLANPacket_MgtFrame::createBefore(p);
    beacon_->destinationAddress() = MACAddress::Broadcast;
    beacon_->sequenceNumber(0u);
    beacon_->bssid() = senf::MACAddress::from_string("00:11:23:32:11:00");
}

prefix_ void senf::emu::detail::EmulatedWLANBeaconGenerator::beaconInterval(ClockService::clock_type interval)
{
    if (ClockService::in_nanoseconds(interval) == 0) {
        timer_.enabled(false);
        return;
    }
    WLANBeaconPacket wbp (beacon_.find<WLANBeaconPacket>(senf::nothrow));
    wbp->beaconInterval() = ClockService::in_milliseconds(timer_.interval().first);
    timer_.interval(interval);
}

prefix_ senf::ClockService::clock_type
senf::emu::detail::EmulatedWLANBeaconGenerator::beaconInterval()
{
    return timer_.interval().first;
}

prefix_ void senf::emu::detail::EmulatedWLANBeaconGenerator::tick()
{
    if (iface_.mode() == EmulatedWLANInterface::STA)
        return;

    beacon_->sourceAddress() = iface_.id();  // iface not known during init
    beacon_->sequenceNumber(beacon_->sequenceNumber()+1);

    beacon_.finalizeAll();
    output(beacon_.clone());
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
