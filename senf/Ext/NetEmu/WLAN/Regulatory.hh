// $Id:$
//
// Copyright (C) 2014
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
    \brief Regulatory header */

#ifndef HH_SENF_Ext_NetEmu_WLAN_Regulatory_
#define HH_SENF_Ext_NetEmu_WLAN_Regulatory_ 1

// Custom includes
#include <cstdint>
#include <senf/Utils/Console/Console.hh>

extern "C" {
#include "nl80211-new.h"
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace senf {
namespace emu {

    class RegulatoryRule
    {
    public:
        enum Flags : std::uint32_t {
            None      = 0,
            NoOFDM    = NL80211_RRF_NO_OFDM,
            NoCCK     = NL80211_RRF_NO_CCK,
            NoIndoor  = NL80211_RRF_NO_INDOOR,
            NoOutdoor = NL80211_RRF_NO_OUTDOOR,
            DFS       = NL80211_RRF_DFS,
            PTPOnly   = NL80211_RRF_PTP_ONLY,
            PTMPOnly  = NL80211_RRF_PTMP_ONLY,
            NoIR      = NL80211_RRF_NO_IR,
            AutoBW    = NL80211_RRF_AUTO_BW
        };

        RegulatoryRule();

        RegulatoryRule & frequencyRange(std::uint32_t lower, std::uint32_t upper);
        RegulatoryRule & maxBandwidth(std::uint32_t bandwidth);
        RegulatoryRule & maxAntennaGain(std::int32_t gain);
        RegulatoryRule & maxEIRP(std::int32_t eirp);
        RegulatoryRule & cacTime(std::uint32_t cac);

        RegulatoryRule & noOFDM(bool flag = true);
        RegulatoryRule & noCCK(bool flag = true);
        RegulatoryRule & noIndoor(bool flag = true);
        RegulatoryRule & noOutdoor(bool flag = true);
        RegulatoryRule & dfsRequired(bool flag = true);
        RegulatoryRule & ptpOnly(bool flag = true);
        RegulatoryRule & ptmpOnly(bool flag = true);
        RegulatoryRule & noIR(bool flag = true);
        RegulatoryRule & autoBw(bool flag = true);
        RegulatoryRule & flags(std::uint32_t f);

        std::uint32_t frequencyRangeBegin() const;
        std::uint32_t frequencyRangeEnd() const;
        std::uint32_t maxBandwidth() const;
        std::int32_t maxAntennaGain() const;
        std::int32_t maxEIRP() const;
        std::uint32_t cacTime() const;

        bool noOFDM() const;
        bool noCCK() const;
        bool noIndoor() const;
        bool noOutdoor() const;
        bool dfsRequired() const;
        bool ptpOnly() const;
        bool ptmpOnly() const;
        bool noIR() const;
        bool autoBw() const;
        std::uint32_t flags() const;

        bool operator==(RegulatoryRule const & other) const;

    private:
        // starting frequency for the regulatory rule in KHz;
        // this is *not* a center of frequency but an actual regulatory band edge:
        std::uint32_t frequencyRangeBegin_;
        // ending frequency for the regulatory rule in KHz;
        // this is *not* a center a frequency but an actual regulatory band edge:
        std::uint32_t frequencyRangeEnd_;
        // maximum allowed bandwidth for this frequency range, in KHz.
        std::uint32_t maxBandwidth_;
        // maximum allowed antenna gain in mBi (100 * dBi).
        std::int32_t maxAntennaGain_;
        // maximum allowed EIRP in mBm (100 * dBm).
        std::int32_t maxEIRP_;
        std::uint32_t flags_;
        // the required CAC time
        std::uint32_t cacTime_;
        
        SENF_CONSOLE_PARSE_FRIEND( RegulatoryRule );
    };

    std::ostream & operator<<(std::ostream & os, RegulatoryRule const & rule);

    SENF_CONSOLE_REGISTER_ENUM_MEMBER( RegulatoryRule, Flags,
            (NoOFDM)(NoCCK)(NoIndoor)(NoOutdoor)(DFS)(PTPOnly)(PTMPOnly)(NoIR)(AutoBW) );


    struct RegulatoryDomain
    {
        enum struct DFSRegion : std::uint8_t {
            Unset = NL80211_DFS_UNSET,
            FCC   = NL80211_DFS_FCC,
            ETSI  = NL80211_DFS_ETSI,
            JP    = NL80211_DFS_JP
        };

        std::string alpha2Country;
        DFSRegion dfsRegion;
        std::vector<RegulatoryRule> rules;

        RegulatoryDomain() { 
            dfsRegion = DFSRegion::Unset; 
        };

        bool isEqual(RegulatoryDomain const & other) const;
    };

    SENF_CONSOLE_REGISTER_STRONG_ENUM_MEMBER( RegulatoryDomain, DFSRegion,
            (key("DFS_Unset", Unset))(key("DFS_FCC", FCC))(key("DFS_ETSI", ETSI))(key("DFS_JP", JP)) );

}

namespace console {

    template <>
    struct ArgumentTraits<emu::RegulatoryRule>
    {
        typedef emu::RegulatoryRule type;
        static bool const singleToken = false;

        static void parse(ParseCommandInfo::TokensRange const & tokens, type & out);
        static std::string description();
        static std::string str(type const & value);
    };

    template <>
    struct ReturnValueTraits<emu::RegulatoryRule>
    {
        typedef emu::RegulatoryRule type;

        static void format(type const & info, std::ostream & os);
    };

}}

SENF_CONSOLE_PARSE_AND_FORMAT_AS_TUPLE( senf::emu::RegulatoryDomain, (alpha2Country)(dfsRegion)(rules) );

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "Regulatory.cci"
//#include "Regulatory.ct"
//#include "Regulatory.cti"
#endif
