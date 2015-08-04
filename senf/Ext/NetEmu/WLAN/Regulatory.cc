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
    \brief Regulatory implementation */

#include "Regulatory.hh"

// Custom includes
#include <senf/Utils/Console/Utility.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::emu::RegulatoryRule

prefix_ senf::emu::RegulatoryRule::RegulatoryRule()
    : frequencyRangeBegin_(0), frequencyRangeEnd_(0), maxBandwidth_(0), maxAntennaGain_(0), maxEIRP_(0), flags_(0), cacTime_(0)
{}

prefix_ std::uint32_t senf::emu::RegulatoryRule::frequencyRangeBegin()
    const
{
    return frequencyRangeBegin_;
}

prefix_ std::uint32_t senf::emu::RegulatoryRule::frequencyRangeEnd()
    const
{
    return frequencyRangeEnd_;
}

prefix_ std::uint32_t senf::emu::RegulatoryRule::maxBandwidth()
    const
{
    return maxBandwidth_;
}

prefix_ std::int32_t senf::emu::RegulatoryRule::maxEIRP()
    const
{
    return maxEIRP_;
}

prefix_ std::uint32_t senf::emu::RegulatoryRule::cacTime()
    const
{
    return cacTime_;
}

prefix_ std::int32_t senf::emu::RegulatoryRule::maxAntennaGain()
    const
{
    return maxAntennaGain_;
}

#define FLAG_GETSET_IMPL( member, flag)                                                 \
    prefix_ bool senf::emu::RegulatoryRule::member()                                    \
        const                                                                           \
    {                                                                                   \
        return flags_ & Flags::flag;                                                    \
    }                                                                                   \
    prefix_ senf::emu::RegulatoryRule & senf::emu::RegulatoryRule::member(bool f)       \
    {                                                                                   \
        if (f) flags_ |= Flags::flag;                                                   \
        else   flags_ &= ~Flags::flag;                                                  \
        return *this;                                                                   \
    }

FLAG_GETSET_IMPL( noOFDM,      NoOFDM    )
FLAG_GETSET_IMPL( noCCK,       NoCCK     )
FLAG_GETSET_IMPL( noIndoor,    NoIndoor  )
FLAG_GETSET_IMPL( noOutdoor,   NoOutdoor )
FLAG_GETSET_IMPL( dfsRequired, DFS       )
FLAG_GETSET_IMPL( ptpOnly,     PTPOnly   )
FLAG_GETSET_IMPL( ptmpOnly,    PTMPOnly  )
FLAG_GETSET_IMPL( noIR,        NoIR      )
FLAG_GETSET_IMPL( autoBw,      AutoBW    )

#undef FLAG_GETSET_IMPL

prefix_ std::uint32_t senf::emu::RegulatoryRule::flags()
    const
{
    return flags_;
}

prefix_ senf::emu::RegulatoryRule & senf::emu::RegulatoryRule::frequencyRange(std::uint32_t begin, std::uint32_t end)
{
    frequencyRangeBegin_ = begin;
    frequencyRangeEnd_ = end;
    return *this;
}

prefix_ senf::emu::RegulatoryRule & senf::emu::RegulatoryRule::maxBandwidth(std::uint32_t bandwidth)
{
    maxBandwidth_ = bandwidth;
    return *this;
}

prefix_ senf::emu::RegulatoryRule & senf::emu::RegulatoryRule::maxEIRP(std::int32_t eirp)
{
    maxEIRP_ = eirp;
    return *this;
}

prefix_ senf::emu::RegulatoryRule & senf::emu::RegulatoryRule::cacTime(std::uint32_t cac)
{
    cacTime_ = cac;
    return *this;
}

prefix_ senf::emu::RegulatoryRule & senf::emu::RegulatoryRule::maxAntennaGain(std::int32_t gain)
{
    maxAntennaGain_ = gain;
    return *this;
}

prefix_ senf::emu::RegulatoryRule & senf::emu::RegulatoryRule::flags(std::uint32_t f)
{
    flags_ = f;
    return *this;
}

prefix_ bool senf::emu::RegulatoryRule::operator==(RegulatoryRule const & other)
    const
{
    return frequencyRangeBegin_ == other.frequencyRangeBegin_ &&
        frequencyRangeEnd_      == other.frequencyRangeEnd_   && 
        maxBandwidth_           == other.maxBandwidth_        &&
        maxAntennaGain_         == other.maxAntennaGain_      &&
        maxEIRP_                == other.maxEIRP_             &&
        cacTime_                == other.cacTime_             &&
        flags_                  == other.flags_;
}

prefix_ bool senf::emu::RegulatoryRule::operator<(RegulatoryRule const & other)
    const
{
    if (frequencyRangeBegin_ < other.frequencyRangeBegin_)
        return true;

    if (frequencyRangeEnd_ < other.frequencyRangeEnd_)
        return true;

    if (maxBandwidth_ < other.maxBandwidth_)
        return true;   

    if (maxAntennaGain_ < other.maxAntennaGain_)
        return true;   

    if (maxEIRP_ < other.maxEIRP_)
        return true;   

    if (cacTime_ < other.cacTime_)
        return true;

    if (flags_ < other.flags_)
        return true;

    return false;
}

prefix_ bool senf::emu::RegulatoryRule::isEqualKernel(RegulatoryRule const & other)
    const
{
    //
    // The kernel (at least 3.16.x) does not set/report CAC time and maxAntennaGain properly.
    // Hence we skip those in the comparison
    //
    return frequencyRangeBegin_ == other.frequencyRangeBegin_ &&
        frequencyRangeEnd_      == other.frequencyRangeEnd_   && 
        maxBandwidth_           == other.maxBandwidth_        &&
        maxEIRP_                == other.maxEIRP_             &&
        flags_                  == other.flags_;
}

prefix_ senf::emu::RegulatoryDomain::operator bool()
    const
{
    return !rules.empty();
}

prefix_ bool senf::emu::RegulatoryDomain::operator==(RegulatoryDomain const & other)
    const
{
    if (dfsRegion != other.dfsRegion)
        return false;

    if (rules.size() != other.rules.size())
        return false;

    for(auto const & r: rules) {
        if (other.rules.find(r) == other.rules.end())
            return false;
    }

    return true;
}

prefix_ bool senf::emu::RegulatoryDomain::operator<(RegulatoryDomain const & other) 
    const
{
    //
    // It's not quite cleat what '<' means for this object, but std::set requires this operator
    // 
    
    std::set<RegulatoryRule>::iterator left (rules.begin());
    std::set<RegulatoryRule>::iterator right (other.rules.begin());
    while ((left != rules.end()) && (right != other.rules.end())) {
        if (*left < *right)
            return true;
        left++;
        right++;
    }

    if (rules.size() < other.rules.size())
        return true;

    if (int(dfsRegion) < int(other.dfsRegion))
        return true;

    return false;
}

prefix_ bool senf::emu::RegulatoryDomain::isEqualKernel(RegulatoryDomain const & other) 
    const
{
    if (dfsRegion != other.dfsRegion)
        return false;

    if (rules.size() != other.rules.size())
        return false;

    for(auto const & r: rules) {
        bool match (false);
        for(auto const & o: other.rules) {
            if (r.isEqualKernel(o)){
                match = true;
                break;
            }
        }
        if (!match)
            return false;
    }
    
    return true;
}


prefix_ std::ostream & senf::emu::operator<<(std::ostream & os, RegulatoryDomain const & regDomain)
{
    senf::console::format(regDomain, os);
    return os;
}

prefix_ std::ostream & senf::emu::operator<<(std::ostream & os, RegulatoryRule const & rule)
{
    os << '(' << rule.frequencyRangeBegin() << ' ' << rule.frequencyRangeEnd() << ' '
       << rule.maxBandwidth() << ' ' << rule.maxEIRP() << ' ' << rule.cacTime() << ' ';
    senf::console::FlagCollection<RegulatoryRule::Flags> flags (rule.flags());
    senf::console::format(flags, os);
    os << ')';
    return os;
}

prefix_ void senf::console::ArgumentTraits<senf::emu::RegulatoryRule>::parse(
        senf::console::ParseCommandInfo::TokensRange const & tokens, senf::emu::RegulatoryRule & rule)
{
    senf::console::CheckedArgumentIteratorWrapper arg (tokens);
    senf::console::parse( *(arg++), rule.frequencyRangeBegin_ );
    senf::console::parse( *(arg++), rule.frequencyRangeEnd_ );
    senf::console::parse( *(arg++), rule.maxBandwidth_ );
    senf::console::parse( *(arg++), rule.maxEIRP_ );
    senf::console::parse( *(arg++), rule.cacTime_ );
    senf::console::FlagCollection<senf::emu::RegulatoryRule::Flags> flags;
    senf::console::parse( *(arg++), flags );
    rule.flags_ = flags;
}

prefix_ std::string senf::console::ArgumentTraits<senf::emu::RegulatoryRule>::description()
{
    return "RegulatoryRule";
};

prefix_ std::string senf::console::ArgumentTraits<senf::emu::RegulatoryRule>::str(type const & info)
{
    std::stringstream ss;
    ss << info;
    return ss.str();
};

prefix_ void senf::console::ReturnValueTraits<senf::emu::RegulatoryRule>::format(
        type const & info, std::ostream & os)
{
    os << info;
};

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
