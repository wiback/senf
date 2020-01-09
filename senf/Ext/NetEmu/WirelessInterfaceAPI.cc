// $Id:$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fit.fraunhofer.de/license.html
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
    \brief WirelessInterfaceAPI non-inline non-template implementation */

#include "WirelessInterfaceAPI.hh"
#include "WLAN/HardwareWLANInterface.hh"

// Custom includes
#include <senf/Utils/String.hh>
#include <senf/Utils/algorithm.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::ModulationParameter & -Registry

prefix_ senf::emu::ModulationParameter::ModulationParameter(std::string const & _coding, short _minRssi,
        unsigned _rate, unsigned _bandwidth, boost::uint8_t _linkTypeId, std::string const & _info)
    : coding(_coding), minRssi(_minRssi), rate(_rate), bandwidth(_bandwidth), linkTypeId(_linkTypeId), info(_info)
{}

prefix_ senf::emu::ModulationParameter::id_t senf::emu::ModulationParameterRegistry::registerModulation(ModulationParameter & parameter)
{
    parameter.id = boost::uint32_t(parameter.linkTypeId) << 24 | boost::uint32_t(parameter.v_modulationId());
    if (modulations_.count(parameter.id) == 0)
        modulations_.insert(parameter.id, parameter.v_clone());
    return parameter.id;
}

prefix_ senf::emu::ModulationParameter const & senf::emu::ModulationParameterRegistry::findModulationById(ModulationParameter::id_t id)
    const
{
    Modulations::const_iterator it (modulations_.find(id));
    if (it == modulations_.end())
        throw ModulationParameter::Error() << id << " (unknown id)";
    return *(it->second);
}

prefix_ senf::emu::ModulationParameter::Error::Error()
    : senf::Exception("Invalid modulation parameter: ")
{}

prefix_ void senf::emu::ModulationParameter::dump(std::ostream & os)
    const
{
    os << "       id: " << id << std::endl
       << "   coding: " << coding << std::endl
       << "     rate: " << rate << std::endl
       << "bandwidth: " << bandwidth << std::endl
       << "min. RSSI: " << minRssi << std::endl;
    v_dump(os);
}

prefix_ void senf::emu::ModulationParameter::dumpTableHeader(std::ostream & os)
    const
{
    boost::format fmt ("%10d%1.1s %11.11s %7d %9d %8d ");
    os << fmt % "id" % "" % "coding" % "rate" % "bandwidth" % "min.RSSI";
    v_dumpTableHeader(os);
    os << std::endl;
}

prefix_ void senf::emu::ModulationParameter::dumpTableRow(std::ostream & os, bool is_current_modulation)
    const
{
    boost::format fmt ("%10d%1.1s %11.11s %7d %9d %8d ");
    os << fmt % id % (is_current_modulation ? "*" : "") % coding % rate % bandwidth % minRssi;
    v_dumpTableRow(os);
    os << std::endl;
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::WirelessInterface

prefix_ void senf::emu::WirelessInterface::init()
{
    Interface::init();

    namespace fty = console::factory;

    consoleDir().add("modulations",
            fty::Command(&WirelessInterface::listModulations, this)
        .doc("List wireless parameter configurations.") );
    consoleDir().add("frequencies",
            fty::Command(&WirelessInterface::listFrequencies, this)
        .doc("List wireless frequencies and bandwidths") );
    consoleDir().add("frequency",
            fty::Command(SENF_MEMBINDFNP(void, WirelessInterface, frequency, (unsigned)))
        .arg("frequency", "frequency of new channel")
        .doc("Change to new channel given by frequency. The bandwidth is not changed.\n"
                "For valid frequencies and bandwidths, see 'frequencies'") );
    consoleDir().add("frequency",
            fty::Command(SENF_MEMBINDFNP(void, WirelessInterface, frequency, (unsigned,unsigned)))
        .arg("frequency", "frequency of new channel")
        .arg("bandwidth", "new channel bandwidth")
        .doc("Change to new channel given by frequency and bandwidth. For valid frequencies and\n"
                "bandwidths, see 'frequencies'") );
    consoleDir().add("whitelistedFrequencyRange",
            fty::Command(&WirelessInterface::whitelistedFrequencyRange, this)
        .arg("frequencyLower", "frequency interval lower bound")
        .arg("frequencyUpper", "frequency interval upper bound")
        .doc("add a whitelisted frequency range") );
    consoleDir().add("whitelistedChannel",
            fty::Command(&WirelessInterface::whitelistedChannel, this)
        .arg("center frequency", "center frequency of the whitelisted channel in kHz")
        .arg("bandwidth", "bandwidth in kHz")
        .doc("add a whitelisted channel") );
    consoleDir().add("clearWhitelistedFrequencies",
            fty::Command(&WirelessInterface::clearWhitelistedFrequencies, this)
        .doc("clear all configured whitelisted frequencies") );
    consoleDir().add("coverageRange",
            fty::Command(&WirelessInterface::coverageRange, this)
        .arg("distance", "distance in meters")
        .doc("set coverage range") );
}

prefix_ void senf::emu::WirelessInterface::listModulations(std::ostream & os)
    const
{
    ModulationParameterIds const & modIds (modulationParameterIds());
    if (modIds.empty()) {
        os << "no modulations configured" << std::endl;
        return;
    }
    ModulationParameterRegistry const & registry (ModulationParameterRegistry::instance());
    ModulationParameter::id_t cmid (0);
    if (canTransmit()) {
        cmid = transmitter().modulation();
        os << "Current modulation: " << std::endl;
        registry.findModulationById(cmid).dump(os);
        os << std::endl;
    }
    registry.findModulationById( *modIds.begin()).dumpTableHeader(os);
    for (ModulationParameter::id_t mid : modIds) {
        registry.findModulationById(mid).dumpTableRow(os, mid==cmid);
    }
}

prefix_ void senf::emu::WirelessInterface::listFrequencies(std::ostream & os)
    const
{
    unsigned bw (bandwidth());
    unsigned freq (frequency());
    os << "Frequency: " << freq
       << "\nBandwidth: " << bw << "\n\n";
    boost::format fmt ("%3d%1.1s %10d%1.1s%-10d %10d%1.1s%-10d\n");
    os << fmt
        % "#" % "" % "FREQ" % "U" % "ENCY" % "BAND" % "W" % "IDTH";
    unsigned n (0);
    for (Frequency const & f : frequencies()) {
        os << fmt
            % n++
            % (f.bandwidthLower <= bw && f.bandwidthUpper >= bw &&
               f.frequencyLower <= freq && f.frequencyUpper >= freq ? "*" : "")
            % f.frequencyLower
            % (f.frequencyLower == f.frequencyUpper ? "" : "-")
            % (f.frequencyLower == f.frequencyUpper ? "" : senf::str(f.frequencyUpper))
            % f.bandwidthLower
            % (f.bandwidthLower == f.bandwidthUpper ? "" : "-")
            % (f.bandwidthLower == f.bandwidthUpper ? "" : senf::str(f.bandwidthUpper));
    }
    os << "Whitelisted Frequency Ranges:\n";
    for (FrequencyInterval const & f : whitelistedFrequencies_) {
        os << f.first << " - " << f.second << std::endl;
    }
}

prefix_ void senf::emu::WirelessInterface::lookupFrequency(unsigned freq, unsigned bw)
    const
{
    if (! frequency_is_whitelisted(whitelistedFrequencies_)(freq-bw/2, freq+bw/2))
        throw InvalidArgumentException("invalid freq, bw ") << freq << ", " << bw << " (not whitelisted)";

    // for h/w wlan we allow fractional bandwidths settings to support 20/40 MHz scans
    // So just stop here. If the channel is complete bogus, the NL call will fail.
    // this needs a major cleanup
    if (dynamic_cast<senf::emu::HardwareWLANInterface const *>(this) != NULL)
        return;    
    
    Frequencies::const_iterator i (
        frequencies_.project<ByLower>(
            frequencies_.get<ByUpper>().lower_bound(boost::make_tuple(bw, freq))));
    Frequencies::const_iterator i_end (
        frequencies_.get<ByLower>().upper_bound(boost::make_tuple(bw, freq)));

    for (; i != i_end; ++i)
        if (bw >= i->bandwidthLower && bw <= i->bandwidthUpper &&
            ((freq == i->frequencyLower && freq == i->frequencyUpper) ||
             (freq >= i->frequencyLower+bw/2  && freq <= i->frequencyUpper-bw/2)))
            return;

    throw InvalidArgumentException("invalid freq, bw ") << freq << ", " << bw;
}

prefix_ senf::emu::WirelessInterface::frequency_is_whitelisted::frequency_is_whitelisted(FrequencyIntervals const & whitelistedFrequencies)
    : whitelistedFrequencies_(whitelistedFrequencies)
{}

prefix_ bool senf::emu::WirelessInterface::frequency_is_whitelisted::operator()(unsigned frequencyLower, unsigned frequencyUpper)
{
    if (whitelistedFrequencies_.empty())
        return true;
    for (FrequencyInterval const & freqInterval : whitelistedFrequencies_) {
        if (freqInterval.first <= frequencyLower && freqInterval.second >= frequencyUpper)
            return true;
    }
    return false;
}

prefix_ bool senf::emu::WirelessInterface::frequency_is_whitelisted::operator()(Frequency const & frequency)
{
    SENF_ASSERT( frequency.frequencyLower == frequency.frequencyUpper,
            "complex frequency ranges are not supported for whitelisting. Sorry.");
    SENF_ASSERT( frequency.bandwidthLower == frequency.bandwidthUpper,
            "complex frequency ranges are not supported for whitelisting. Sorry.");
    return operator()( frequency.frequencyLower - frequency.bandwidthLower/2,
            frequency.frequencyLower + frequency.bandwidthLower/2 );
}

prefix_ void senf::emu::WirelessInterface::whitelistedFrequencyRange(unsigned frequencyLower, unsigned frequencyUpper)
{
    if (frequencyUpper <= frequencyLower)
        throw InvalidArgumentException("frequencyUpper (") << frequencyUpper << ") <= frequencyLower (" << frequencyLower << ")";
    whitelistedFrequencies_.insert( std::make_pair(frequencyLower, frequencyUpper));
}

prefix_ void senf::emu::WirelessInterface::whitelistedChannel(unsigned frequency, unsigned bandwidth)
{
    if (frequency < bandwidth/2)
        throw InvalidArgumentException("frequency (") << frequency << ") < bandwidth/2 (" << bandwidth/2 << ")";
    whitelistedFrequencies_.insert( std::make_pair(frequency-bandwidth/2, frequency+bandwidth/2));
}

prefix_ void senf::emu::WirelessInterface::clearWhitelistedFrequencies()
{
    whitelistedFrequencies_.clear();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::WirelessTransmitter

prefix_ void senf::emu::WirelessTransmitter::init()
{
    Transmitter::init();

    namespace fty = console::factory;

    interface().consoleDir()
        .add("txpower", fty::Command(SENF_MEMBINDFNP(void, WirelessTransmitter, txPower, (int)))
             .arg("power", "new transmit power")
             .doc("Set current transmit power. Use 'txpowers' to list valid values or\n"
                  "get the current txpower value.") );
    interface().consoleDir()
        .add("txpowers", fty::Command(&WirelessTransmitter::listTxpowers, this)
        .doc("List valid transmit power values for current parameter configuration.\n"
             "This lists depends on the selected parameter configuration !\n"
             "\n"
             "The unit of the value listed in the POWER column is wireless interface type and\n"
             "driver dependent.") );
}

prefix_ void senf::emu::WirelessTransmitter::listTxpowers(std::ostream & os)
    const
{
    int txp (txPower());
    os << "txPower: " << txp << "\n\n";
    boost::format fmt ("%3d%1.1s %9d%1.1s%-9d\n");
    os << fmt % "#" % "" % "PO" % "W" % "ER";
    TxPowersRange p (txPowers());
    unsigned n (0);
    for (TxPowersRange::iterator i (p.begin()); i != p.end(); ++i, ++n)
        os << fmt
            % n
            % (i->lower <= txp && i->upper >= txp ? "*" : "")
            % i->lower
            % (i->lower == i->upper ? "" : "-")
            % (i->lower == i->upper ? "" : senf::str(i->upper));
}

prefix_ senf::emu::ModulationParameter::id_t senf::emu::WirelessTransmitter::modulation()
    const
{
    return v_modulationId();
}

prefix_ void senf::emu::WirelessTransmitter::modulation(ModulationParameter::id_t id)
{
    if (! senf::contains(interface().modulationParameterIds(), id))
        throw ModulationParameter::Error() << id;
    v_modulationId(id);
}

prefix_ void senf::emu::WirelessTransmitter::registerTxPower(int power)
{
    txPowers_.insert(TxPower(power, power));
}

prefix_ void senf::emu::WirelessTransmitter::registerTxPower(int powerLower, int powerUpper)
{
    if (powerLower >= powerUpper)
        throw InvalidArgumentException("Reversed power parameter in senf::emu::WirelessTransmitter::registerTxPower() ")
            << powerLower << " >= " << powerUpper;
    txPowers_.insert(TxPower(powerLower, powerUpper));
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
