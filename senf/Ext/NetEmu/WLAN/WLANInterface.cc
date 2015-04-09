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
//       Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>

/** \file
    \brief WLANInterface non-inline non-template implementation */

#include "WLANInterface.hh"

// Custom includes
#include <boost/foreach.hpp>
#include <senf/Packets/80211Bundle/MCSInfo.hh>
#include <senf/Utils/String.hh>
#include <senf/Utils/senflikely.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::WLANModulationParameter / -Registry

namespace {
    static senf::emu::WLANModulationParameterRegistry::LegacyModulationInfo legacyInfos11b[] = {
        { "BPSK/DSSS",  1000, -82, },
        { "QPSK/DSSS",  2000, -82, },
        { "CCK55",      5500, -82, },
        { "CCK11",     11000, -82, },
    };
    static senf::emu::WLANModulationParameterRegistry::LegacyModulationInfo legacyInfosOFDM[] = {
        { "BPSK 1/2",   6000, -81, },
        { "BPSK 3/4",   9000, -81, },
        { "QPSK 1/2",  12000, -79, },
        { "QPSK 3/4",  18000, -77, },
        { "16QAM 1/2", 24000, -74, },
        { "16QAM 3/4", 36000, -70, },
        { "64QAM 2/3", 48000, -66, },
        { "64QAM 3/4", 54000, -65, },
    };
}

prefix_ std::vector<senf::emu::WLANModulationParameterRegistry::LegacyModulationInfo>
senf::emu::WLANModulationParameterRegistry::getLegacyModulationInfos11b()
{
    return std::vector<LegacyModulationInfo>(legacyInfos11b, legacyInfos11b + sizeof(legacyInfos11b) / sizeof(LegacyModulationInfo));
}

prefix_ std::vector<senf::emu::WLANModulationParameterRegistry::LegacyModulationInfo>
senf::emu::WLANModulationParameterRegistry::getLegacyModulationInfosOFDM()
{
    return std::vector<LegacyModulationInfo>(legacyInfosOFDM, legacyInfosOFDM + sizeof(legacyInfosOFDM) / sizeof(LegacyModulationInfo));
}

prefix_ senf::emu::WLANModulationParameterRegistry::WLANModulationParameterRegistry()
{
    // register HT modulations
    BOOST_FOREACH( senf::WLAN_MCSInfo::Info const & info, senf::WLAN_MCSInfo::getInfos() ) {
        for (unsigned i=0; i<4; ++i) {
            WLANModulationParameter p (
                    info.coding, info.rssi[i<2?0:1], info.rate[i], (i<2 ? 20000 : 40000),
                    WLANModulationParameter::MCS, info.streams, info.index, (i%2==1));
            mcsParametersToId_.push_back( registerModulation( p));
        }
    }
    // register non-HT legacy modulations
    BOOST_FOREACH( LegacyModulationInfo mod, legacyInfos11b ) {
        WLANModulationParameter p (mod.coding, mod.rssi, mod.rate, 20000, WLANModulationParameter::Legacy);
        rateToId_[mod.rate] = registerModulation( p);
    }
    BOOST_FOREACH( LegacyModulationInfo mod, legacyInfosOFDM ) {
        WLANModulationParameter p (mod.coding, mod.rssi, mod.rate, 20000, WLANModulationParameter::Legacy);
        rateToId_[mod.rate] = registerModulation( p);
    }
    // register the two 'special' modulations
    {
        WLANModulationParameter p ("auto", 0, 0, 0, WLANModulationParameter::Automatic, 0, 0, false);
        parameterIdAuto_ = registerModulation( p);
    } {
        WLANModulationParameter p ("unknown", 0, 0, 0, WLANModulationParameter::Unknown, 0, 0, false);
        parameterIdUnknown_ = registerModulation( p);
    }
}

prefix_ boost::uint32_t senf::emu::WLANModulationParameterRegistry::registerModulation(WLANModulationParameter & p)
{
    ModulationParameter::id_t id = ModulationParameterRegistry::instance().registerModulation( p);
    modulations_.insert( std::make_pair(id, p));
    return id;
}

prefix_ senf::emu::WLANModulationParameter const & senf::emu::WLANModulationParameterRegistry::findModulationById(ModulationParameter::id_t id)
    const
{
    Modulations::const_iterator it (modulations_.find(id));
    if (it == modulations_.end())
        throw ModulationParameter::Error() << id << " (unknown id)";
    return it->second;
}

prefix_ senf::emu::ModulationParameter::id_t senf::emu::WLANModulationParameterRegistry::parameterIdByLegacyRate(unsigned rate)
    const
{
    RateToIdMap::const_iterator i (rateToId_.find(rate));
    return (i != rateToId_.end() ? i->second : 0);
}

prefix_ senf::emu::ModulationParameter::id_t senf::emu::WLANModulationParameterRegistry::parameterIdByMCS(unsigned mcsIndex, bool ht40, bool shortGI)
    const
{
    if (SENF_UNLIKELY(mcsIndex+3 > mcsParametersToId_.size()))
        return 0;
    return mcsParametersToId_[(mcsIndex * 4) + (ht40 * 2u) + (shortGI * 1u)];
}

prefix_ senf::emu::ModulationParameter::id_t senf::emu::WLANModulationParameterRegistry::parameterIdAuto()
    const
{
    return parameterIdAuto_;
}

prefix_ senf::emu::ModulationParameter::id_t senf::emu::WLANModulationParameterRegistry::parameterIdUnknown()
    const
{
    return parameterIdUnknown_;
}

prefix_ senf::emu::WLANModulationParameter::WLANModulationParameter(std::string const & _label, short _minRssi,
        unsigned _rate, unsigned _bandwidth, Type _type, unsigned _streams, unsigned _mcsIndex,  bool _shortGI)
    : ModulationParameter(_label, _minRssi, _rate, _bandwidth, WLANInterface::linkTypeId),
      type(_type), mcsIndex(_mcsIndex), streams(_streams), shortGI(_shortGI)
{}

prefix_ boost::uint16_t senf::emu::WLANModulationParameter::v_modulationId()
    const
{
    switch (type) {
    case Legacy:
        return 100 + rate;
    case MCS:
        return 2 + (mcsIndex * 4) + (bandwidth==40000 ? 2 : 0) + (shortGI ? 1 : 0);
    case Automatic:
        return 1;
    case Unknown:
        return 0;
    }
    return 0;
}

prefix_ senf::emu::ModulationParameter * senf::emu::WLANModulationParameter::v_clone()
    const
{
    return new WLANModulationParameter(*this);
}

prefix_ void senf::emu::WLANModulationParameter::v_dump(std::ostream & os)
    const
{
    static const char * types[] = { "Legacy", "MCS", "Automatic", "Unknown" };
    os << "     type: " << (type <= Unknown ? types[type] : "?") << std::endl
       << "MCS Index: " << (type==MCS ? senf::str(mcsIndex) : "-") << std::endl
       << "# streams: " << streams << std::endl
       << " short GI: " << (shortGI ? "true" : "false") << std::endl;
}

prefix_ void senf::emu::WLANModulationParameter::v_dumpTableHeader(std::ostream & os)
    const
{
    boost::format fmt ("%3d %7d %8s");
    os << fmt % "mcs" % "streams" % "short GI";
}

prefix_ void senf::emu::WLANModulationParameter::v_dumpTableRow(std::ostream & os)
    const
{
    boost::format fmt ("%3d %7d %8s");
    os << fmt % (type==MCS ? senf::str(mcsIndex) : " - ") % streams % (shortGI ? "true" : "false");
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::WLANInterface

prefix_ void senf::emu::WLANInterface::init()
{
    WirelessInterface::init();
    WirelessReceiver::init();
    WirelessTransmitter::init();

    namespace fty = console::factory;
    consoleDir().add("modulation", fty::Command(
            SENF_MEMBINDFNP(void, WLANInterface, modulation, (WLANModulationParameter::Type, unsigned value) ))
        .arg("type", "'mcs' or 'legacy'")
        .arg("value", "modulation value; mcs index or legacy rate")
        .doc("set modulation") );
    consoleDir().add("modulation", fty::Command(
            SENF_MEMBINDFNP(void, WLANInterface, modulation, (WLANModulationParameter::Type) ))
        .arg("type", "must be 'auto'")
        .doc("set modulation to auto") );
}

prefix_ void senf::emu::WLANInterface::modulation(WLANModulationParameter::Type type, unsigned value)
{
    switch (type) {
    case WLANModulationParameter::MCS:
        modulation( WLANModulationParameterRegistry::instance().parameterIdByMCS(
                value, (bandwidth() == 40000), false));
        return;
    case WLANModulationParameter::Legacy:
        modulation( WLANModulationParameterRegistry::instance().parameterIdByLegacyRate(value));
        return;
    case WLANModulationParameter::Automatic:
        throw InvalidArgumentException("invalid argument for auto-modulation; no argument required.");
    case WLANModulationParameter::Unknown:
        throw InvalidArgumentException("invalid wlan modulation type: unknown");
    }
}

prefix_ void senf::emu::WLANInterface::modulation(WLANModulationParameter::Type type)
{
    switch (type) {
    case WLANModulationParameter::Automatic:
        modulation( WLANModulationParameterRegistry::instance().parameterIdAuto());
        return;
    case WLANModulationParameter::Unknown:
        throw InvalidArgumentException("invalid wlan modulation type: unknown");
    default:
        throw InvalidArgumentException("argument missing for modulation setting.");
    }
}

prefix_ boost::uint8_t senf::emu::WLANInterface::v_linkTypeId()
    const
{
    return linkTypeId;
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
