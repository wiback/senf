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
    \brief DVBInterface non-inline non-template implementation */

#include "DVBInterface.hh"

// Custom includes

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
    using namespace senf::emu;

    struct DVBParam {
        DVBModulationParameter::fec_rate_t fec;
        DVBModulationParameter::modulation_t modulation;
        DVBModulationParameter::guard_interval_t guard_interval;
        std::string coding;
        short rssi;
        unsigned rate;
    };

    // rates calculated with http://www.saschateichmann.de/dvb-t-hf.html
    // min RSSI values are guessed.
    static DVBParam dvbParams[] = {
            { DVBModulationParameter::FEC_3_4, DVBModulationParameter::QAM_16, DVBModulationParameter::GUARD_INTERVAL_1_16, "16QAM 3/4", -70, 17564 },
            { DVBModulationParameter::FEC_1_2, DVBModulationParameter::QAM_16, DVBModulationParameter::GUARD_INTERVAL_1_4,  "16QAM 1/2", -74,  9953 },
            { DVBModulationParameter::FEC_3_4, DVBModulationParameter::QAM_64, DVBModulationParameter::GUARD_INTERVAL_1_32, "64QAM 3/4", -65, 27144 },
    };
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::DVBModulationParameter / -Registry

namespace {
    boost::uint16_t calc_modulationId(DVBModulationParameter::fec_rate_t fec, DVBModulationParameter::modulation_t mod,
            DVBModulationParameter::guard_interval_t gi)
    {
        return (boost::uint16_t(fec) << 8) | (boost::uint16_t(mod) << 4) | (boost::uint16_t(gi));
    }
}

prefix_ senf::emu::DVBModulationParameter::DVBModulationParameter(std::string const & _label, short _minRssi, unsigned _rate,
        fec_rate_t _fec, modulation_t mod, guard_interval_t gi)
    : ModulationParameter(_label, _minRssi, _rate,  8000, /* we assume a fixed bandwidth of 8 MHz */ DVBInterface::linkTypeId),
      fec(_fec), modulation(mod), guard_interval(gi)
{}


prefix_  boost::uint16_t senf::emu::DVBModulationParameter::v_modulationId()
    const
{
    return calc_modulationId(fec, modulation, guard_interval);
}

prefix_  senf::emu::ModulationParameter * senf::emu::DVBModulationParameter::v_clone()
    const
{
    return new DVBModulationParameter(*this);
}

prefix_ void senf::emu::DVBModulationParameter::v_dump(std::ostream & os)
    const
{
    os << "todo!" << std::endl;
}

prefix_ void senf::emu::DVBModulationParameter::v_dumpTableHeader(std::ostream & os)
    const
{
    os << "todo!";
}

prefix_ void senf::emu::DVBModulationParameter::v_dumpTableRow(std::ostream & os)
    const
{
    os << "todo!";
}

prefix_ senf::emu::DVBModulationParameterRegistry::DVBModulationParameterRegistry()
{
    for (DVBParam const & param : dvbParams) {
        DVBModulationParameter p (param.coding, param.rssi, param.rate, param.fec, param.modulation, param.guard_interval);
        registerModulation( p);
    }
}

prefix_ boost::uint32_t senf::emu::DVBModulationParameterRegistry::registerModulation(DVBModulationParameter & p)
{
    ModulationParameter::id_t id = ModulationParameterRegistry::instance().registerModulation( p);
    modulations_.insert( std::make_pair(id, p));
    idTranslateMap_.insert( std::make_pair(p.v_modulationId(), id));
    return id;
}

prefix_ senf::emu::DVBModulationParameter const & senf::emu::DVBModulationParameterRegistry::findModulationById(ModulationParameter::id_t id)
    const
{
    Modulations::const_iterator it (modulations_.find(id));
    if (it == modulations_.end())
        throw ModulationParameter::Error() << id << " (unknown id)";
    return it->second;
}

prefix_ senf::emu::ModulationParameter::id_t senf::emu::DVBModulationParameterRegistry::parameterId(
        DVBModulationParameter::fec_rate_t fec, DVBModulationParameter::modulation_t mod, DVBModulationParameter::guard_interval_t gi)
    const
{
    IdTranslateMap::const_iterator i (idTranslateMap_.find(calc_modulationId(fec, mod, gi)));
    return (i != idTranslateMap_.end() ? i->second : 0);
}

prefix_ senf::emu::DVBModulationParameterRegistry::ModulationParameterIds senf::emu::DVBModulationParameterRegistry::parameterIds()
    const
{
    return senf::make_transform_range(modulations_, ::__gnu_cxx::select1st<Modulations::value_type>());
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::DVBInterface

namespace senf { namespace emu {
    SENF_CONSOLE_REGISTER_ENUM_MEMBER( DVBModulationParameter, fec_rate_t,
            (FEC_NONE)(FEC_1_2)(FEC_2_3)(FEC_3_4)(FEC_4_5)(FEC_5_6)(FEC_6_7)(FEC_7_8)(FEC_8_9)
            (FEC_AUTO)(FEC_3_5)(FEC_9_10) );
    SENF_CONSOLE_REGISTER_ENUM_MEMBER( DVBModulationParameter, modulation_t,
            (QPSK)(QAM_16)(QAM_32)(QAM_64)(QAM_128)(QAM_256)(QAM_AUTO)(VSB_8)(VSB_16)(PSK_8)
            (APSK_16)(APSK_32)(DQPSK) );
    SENF_CONSOLE_REGISTER_ENUM_MEMBER( DVBModulationParameter, guard_interval_t,
            (GUARD_INTERVAL_1_32)(GUARD_INTERVAL_1_16)(GUARD_INTERVAL_1_8)(GUARD_INTERVAL_1_4)
            (GUARD_INTERVAL_AUTO)(GUARD_INTERVAL_1_128)(GUARD_INTERVAL_19_128)(GUARD_INTERVAL_19_256) );
}}

prefix_ void senf::emu::DVBInterface::init()
{
    WirelessInterface::init();

    namespace fty = console::factory;
    consoleDir().add("tune", fty::Command(&DVBInterface::tune, this) );
}

prefix_ boost::uint8_t senf::emu::DVBInterface::v_linkTypeId()
    const
{
    return linkTypeId;
}

prefix_ void senf::emu::DVBInterface::tune(DVBModulationParameter::fec_rate_t fec, DVBModulationParameter::modulation_t mod,
        DVBModulationParameter::guard_interval_t gi, unsigned freq)
{
    v_tune( fec, mod, gi, freq);
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
