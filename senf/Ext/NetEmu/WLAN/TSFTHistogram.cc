//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief TSFT Histogram implementation */

#include "TSFTHistogram.hh"

// Custom includes
#include <senf/Utils/membind.hh>
#include <senf/Utils/Console/ParsedCommand.hh>
#include <senf/Utils/Console/Variables.hh>
#include <senf/Scheduler/Scheduler.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::emu::TSFTHistogram::TSFTHistogram()
    : tsftMapInitialized_(false),
      duration_(senf::ClockService::seconds(60)),
      src_(senf::MACAddress::None),
      enabled_(false)
{
    namespace fty = senf::console::factory;

    dir.add("duration", fty::Variable(duration_)
        .doc( "set the TSFT histogram collection duration. Default 60s."));
    dir.add("start", fty::Command(
                SENF_MEMBINDFNP(bool, TSFTHistogram, start, ()))
        .doc("enables/resets the TSFT Histogram accepting any SRC address."));
    dir.add("start", fty::Command(
                SENF_MEMBINDFNP(bool, TSFTHistogram, start, (senf::MACAddress const &)))
        .doc("enables/resets the TSFT Histogram accepting only the specified SRC address."));
    dir.add("enabled", fty::Command(
                SENF_MEMBINDFNP(bool, TSFTHistogram, enabled, () const))
        .doc("returns the TSFT Histogram state"));
    dir.add("dump", fty::Command(
                SENF_MEMBINDFNP(void, TSFTHistogram, dump, (std::ostream &) const))
        .doc("dumps the current TSFT Histogram"));
}

prefix_ void senf::emu::TSFTHistogram::update(senf::MACAddress const & src, boost::uint64_t const & tsft)
{
    // are we done ?
    if ((senf::scheduler::now() - startTime_) > duration_) {
        enabled_ = false;
        return;
    }

    // first call
    if (!tsftMapInitialized_) {
        tsftMapInitialized_ = true;
        lastTSFT_ = tsft;
        return;
    }
    
    // we either accept all frames, or frame from only one source
    if (src_ && (src != src_)) {
        return;
    }

    boost::uint32_t diff;
    if (tsft >= lastTSFT_) {
        diff =  boost::uint32_t(tsft - lastTSFT_);
    }
    else {
        diff =  boost::uint32_t(boost::uint64_t(-1) - lastTSFT_ + tsft);
    }
    
    TSFTMap::iterator it( tsftMap_.find(diff));
    if (it != tsftMap_.end()) {
        it->second++;
    }
    else {
        tsftMap_.insert( std::make_pair( diff, 1u)); // 1 => count the current packet
    }
    
    lastTSFT_ = tsft;
}


prefix_ bool senf::emu::TSFTHistogram::start()
{
    return start( senf::MACAddress::None);
}

prefix_ bool senf::emu::TSFTHistogram::start( senf::MACAddress const & src)
{
    src_ = src;
    startTime_ = senf::scheduler::now();
    tsftMapInitialized_ = false;
    tsftMap_.clear();
    enabled_ = true;
    return true;
}

prefix_ void senf::emu::TSFTHistogram::dump(std::ostream & os)
    const
{
    os << "=== TSFT Histogram (times in us) of " << src_ << ", age " << senf::ClockService::in_seconds(senf::scheduler::now() - startTime_) << "s ===" << std::endl;
    
    if (tsftMap_.size() == 0) {
        os << "no values collected." << std::endl;
        return;
    }
    
    // current filter: sum of squares to find peaks
    boost::uint64_t avg = 0;
    
    TSFTMap::const_iterator it;
    for( it = tsftMap_.begin(); it != tsftMap_.end(); it++){
        avg += it->second*it->second;
    }
    avg /= tsftMap_.size();
    
    for( it = tsftMap_.begin(); it != tsftMap_.end(); it++){
        if ((it->second * it->second) > avg) {
            os << it->first << "," << it->second << std::endl;
        }
    }
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
