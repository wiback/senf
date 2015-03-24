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
    \brief WLANInterface public header */

#ifndef HH_SENF_Ext_NetEmu_WLAN_WLANInterface_
#define HH_SENF_Ext_NetEmu_WLAN_WLANInterface_ 1

// Custom includes
#include <senf/Ext/NetEmu/WirelessInterfaceAPI.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    class WLANModulationParameterRegistry;


    class WLANModulationParameter
        : public ModulationParameter
    {
    public:
        enum Type { Legacy, MCS, Automatic, Unknown };

        Type type;
        unsigned  mcsIndex;
        unsigned streams;
        bool shortGI;

    private:
        WLANModulationParameter(std::string const & _label, short _minRssi, unsigned _rate,
                unsigned _bandwidth, Type _type, unsigned _streams=1,
                unsigned _mcsIndex=0, bool _shortGI=false);

        virtual boost::uint16_t v_modulationId() const;
        virtual ModulationParameter * v_clone() const;
        virtual void v_dump(std::ostream & os) const;
        virtual void v_dumpTableHeader(std::ostream & os) const;
        virtual void v_dumpTableRow(std::ostream & os) const;

        friend class WLANModulationParameterRegistry;
    };

    SENF_CONSOLE_REGISTER_ENUM_MEMBER( WLANModulationParameter, Type, (Legacy)(MCS)(Automatic) );


    class WLANModulationParameterRegistry
        : private boost::noncopyable
    {
    public:
        static WLANModulationParameterRegistry & instance() {
            static WLANModulationParameterRegistry _instance;
            return _instance;
        }

        struct LegacyModulationInfo {
            std::string coding;
            unsigned rate;
            short rssi;
        };
        static std::vector<LegacyModulationInfo> getLegacyModulationInfos11b();
        static std::vector<LegacyModulationInfo> getLegacyModulationInfosOFDM();

        WLANModulationParameter const & findModulationById(ModulationParameter::id_t id) const;
        ModulationParameter::id_t parameterIdByLegacyRate(unsigned rate) const;
        ModulationParameter::id_t parameterIdByMCS(unsigned mcsIndex, bool ht40, bool shortGI) const;
        ModulationParameter::id_t parameterIdAuto() const;
        ModulationParameter::id_t parameterIdUnknown() const;

    private:
        WLANModulationParameterRegistry();

        ModulationParameter::id_t registerModulation(WLANModulationParameter & p);

        typedef std::map<ModulationParameter::id_t, WLANModulationParameter> Modulations;
        Modulations modulations_;
        std::vector<ModulationParameter::id_t> mcsParametersToId_;
        typedef std::map<unsigned, ModulationParameter::id_t> RateToIdMap;
        RateToIdMap rateToId_;
        ModulationParameter::id_t parameterIdAuto_;
        ModulationParameter::id_t parameterIdUnknown_;
    };


    class WLANInterface
        : public WirelessInterface, public WirelessReceiver, public WirelessTransmitter
    {
    public:
        template <class Output, class Input>
        WLANInterface(Output & output, Input & input);

        static const boost::uint8_t linkTypeId = 19;  // 802.21 link type for IEEE 802.11

        using WirelessTransmitter::modulation;
        void modulation(WLANModulationParameter::Type type, unsigned value);
        void modulation(WLANModulationParameter::Type type);

    protected:
        void init();

    private:
        virtual boost::uint8_t v_linkTypeId() const;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "WLANInterface.cci"
#include "WLANInterface.ct"
//#include "WLANInterface.cti"
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
