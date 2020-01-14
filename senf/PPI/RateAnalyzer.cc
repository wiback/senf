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
    \brief RateAnalyzer non-inline non-template implementation */

#include "RateAnalyzer.hh"
//#include "RateAnalyzer.ih"

// Custom includes

//#include "RateAnalyzer.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::ppi::module::RateAnalyzer::RateAnalyzer()
    : packets_ (0u), bytes_ (0u), minSize_ (0u), maxSize_ (0u), factor_ (0.0f)
{
    registerEvent(timer_, &RateAnalyzer::tick);
}

prefix_ void senf::ppi::module::RateAnalyzer::startStatistics(ClockService::clock_type interval)
{
    timer_.interval(interval);
    factor_ = double(ClockService::in_nanoseconds(interval)) /
        double(ClockService::in_nanoseconds(ClockService::seconds(1)));
}

prefix_ void senf::ppi::module::RateAnalyzer::v_handlePacket(Packet const & p)
{
    ++packets_;
    unsigned sz (p.data().size());
    bytes_ += sz;
    if (sz < minSize_ || minSize_ == 0u) minSize_ = sz;
    if (sz > maxSize_) maxSize_ = sz;
}

prefix_ void senf::ppi::module::RateAnalyzer::tick()
{
    signals.packetsPerSecond(packets_/factor_);
    signals.bytesPerSecond(bytes_/factor_);
    signals.bytesPerPacket(minSize_, float(bytes_)/float(packets_), maxSize_);

    packets_ = bytes_ = minSize_ = maxSize_ = 0;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "RateAnalyzer.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
