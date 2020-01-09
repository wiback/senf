// $Id:$
//
// Copyright (C) 2012
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
//       Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>

/** \file
    \brief DVBInterface public header */

#ifndef HH_SENF_Ext_NetEmu_DVB_DVBInterface_
#define HH_SENF_Ext_NetEmu_DVB_DVBInterface_ 1

// Custom includes
#include "senf/Ext/NetEmu/WirelessInterfaceAPI.hh"

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    class DVBModulationParameterRegistry;


    class DVBModulationParameter
        : public ModulationParameter
    {
    public:
        // enums copied from <linux/dvb/frontend.h>
        enum fec_rate_t {
            FEC_NONE = 0, FEC_1_2, FEC_2_3, FEC_3_4, FEC_4_5, FEC_5_6, FEC_6_7,
            FEC_7_8, FEC_8_9, FEC_AUTO, FEC_3_5, FEC_9_10 };
        enum modulation_t {
            QPSK, QAM_16, QAM_32, QAM_64, QAM_128, QAM_256, QAM_AUTO, VSB_8, VSB_16,
            PSK_8,  APSK_16, APSK_32, DQPSK };
        enum guard_interval_t {
            GUARD_INTERVAL_1_32, GUARD_INTERVAL_1_16, GUARD_INTERVAL_1_8, GUARD_INTERVAL_1_4,
            GUARD_INTERVAL_AUTO, GUARD_INTERVAL_1_128, GUARD_INTERVAL_19_128, GUARD_INTERVAL_19_256 };

        fec_rate_t fec;
        modulation_t modulation;
        guard_interval_t guard_interval;

    private:
        DVBModulationParameter(std::string const & _label, short _minRssi, unsigned _rate,
                fec_rate_t _fec, modulation_t mod, guard_interval_t gi);

        virtual boost::uint16_t v_modulationId() const;
        virtual ModulationParameter * v_clone() const;
        virtual void v_dump(std::ostream & os) const;
        virtual void v_dumpTableHeader(std::ostream & os) const;
        virtual void v_dumpTableRow(std::ostream & os) const;

        friend class DVBModulationParameterRegistry;
    };


    class DVBModulationParameterRegistry
        : private boost::noncopyable
    {
        DVBModulationParameterRegistry();

        ModulationParameter::id_t registerModulation(DVBModulationParameter & p);

        typedef std::map<ModulationParameter::id_t, DVBModulationParameter> Modulations;
        Modulations modulations_;
        typedef std::map<boost::uint16_t, ModulationParameter::id_t> IdTranslateMap;
        IdTranslateMap idTranslateMap_;

    public:
        static DVBModulationParameterRegistry & instance() {
            static DVBModulationParameterRegistry _instance;
            return _instance;
        }

        DVBModulationParameter const & findModulationById(ModulationParameter::id_t id) const;
        ModulationParameter::id_t parameterId(DVBModulationParameter::fec_rate_t fec,
                DVBModulationParameter::modulation_t mod, DVBModulationParameter::guard_interval_t gi) const;

        typedef boost::transform_iterator< ::__gnu_cxx::select1st<Modulations::value_type>,
                Modulations::const_iterator > ModulationParameterId_iterator;
        typedef boost::iterator_range<ModulationParameterId_iterator> ModulationParameterIds;
        ModulationParameterIds parameterIds() const;
    };


    class DVBInterface
        : public WirelessInterface
    {
    public:
        static const boost::uint8_t linkTypeId = 40; // 802.21b link type for DVB

        void tune(DVBModulationParameter::fec_rate_t fec, DVBModulationParameter::modulation_t mod,
                DVBModulationParameter::guard_interval_t gi, unsigned freq);

    protected:
        void init();

    private:
        virtual boost::uint8_t v_linkTypeId() const;
        virtual void v_tune(DVBModulationParameter::fec_rate_t fec, DVBModulationParameter::modulation_t mod,
                DVBModulationParameter::guard_interval_t gi, unsigned frequency) = 0;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "DVBInterface.cci"
//#include "DVBInterface.ct"
//#include "DVBInterface.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
