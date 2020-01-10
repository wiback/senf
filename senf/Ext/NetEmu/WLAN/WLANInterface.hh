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
        enum Type { Legacy, HT, VHT, Automatic, Unknown };

        Type type;
        unsigned  index;
        unsigned streams;
        bool shortGI;

        static boost::uint16_t modulationId(Type _type, unsigned _rate, unsigned _bandwidth,
                unsigned _index, unsigned _streams, bool _shortGI);

    private:
        WLANModulationParameter(std::string const & _label, short _minRssi, unsigned _rate,
                unsigned _bandwidth, Type _type, unsigned _streams=1,
                unsigned _index=0, bool _shortGI=false);

        virtual boost::uint16_t v_modulationId() const;
        virtual ModulationParameter * v_clone() const;
        virtual void v_dump(std::ostream & os) const;
        virtual void v_dumpTableHeader(std::ostream & os) const;
        virtual void v_dumpTableRow(std::ostream & os) const;

        friend class WLANModulationParameterRegistry;
    };

    SENF_CONSOLE_REGISTER_ENUM_MEMBER( WLANModulationParameter, Type, (Legacy)(HT)(VHT)(Automatic) );


    class WLANModulationParameterRegistry
        : private boost::noncopyable
    {
    public:
        static WLANModulationParameterRegistry & instance() {
            static WLANModulationParameterRegistry _instance;
            return _instance;
        }

        enum GIMatch {Any, Equal, Short, Long };

        struct LegacyModulationInfo {
            std::string coding;
            unsigned rate;
            short rssi;
        };
        static std::vector<LegacyModulationInfo> getLegacyModulationInfos11b();
        static std::vector<LegacyModulationInfo> getLegacyModulationInfosOFDM();

        WLANModulationParameter const & findModulationById(ModulationParameter::id_t id) const;
        ModulationParameter::id_t parameterIdByLegacyRate(unsigned rate) const;
        ModulationParameter::id_t parameterIdByMCS_HT(unsigned htMcsIndex, unsigned bandwidth, bool shortGI) const;
        ModulationParameter::id_t parameterIdByMCS_VHT(unsigned vhtMcsIndex, unsigned streams, unsigned bandwidth, bool shortGI) const;
        ModulationParameter::id_t parameterIdAuto() const;
        ModulationParameter::id_t parameterIdUnknown() const;

        std::set<ModulationParameter::id_t> parameterIdsByType(ModulationParameter::id_t id, GIMatch giMatch = Any) const;
        ModulationParameter::id_t parameterIdAsLGI(ModulationParameter::id_t id) const;
        ModulationParameter::id_t parameterIdAsSGI(ModulationParameter::id_t id) const;
        ModulationParameter::id_t parameterIdLower(ModulationParameter::id_t id, unsigned diff) const;

    private:
        WLANModulationParameterRegistry();

        ModulationParameter::id_t registerModulation(WLANModulationParameter & p);

        typedef boost::unordered_map<ModulationParameter::id_t, WLANModulationParameter> Modulations;
        Modulations modulations_;
        boost::unordered_map<boost::uint16_t, ModulationParameter::id_t> idMap_;
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
        void modulation(WLANModulationParameter::Type type, unsigned index, unsigned streams);
        void modulation(WLANModulationParameter::Type type, unsigned value);
        void modulation(WLANModulationParameter::Type type);

        unsigned maxNSS() const;

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
