// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
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
//   Stefan Bund <g0dil@berlios.de>

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
