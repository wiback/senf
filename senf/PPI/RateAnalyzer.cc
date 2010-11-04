// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

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

prefix_ void senf::ppi::module::RateAnalyzer::startStatistics(senf::ClockService::clock_type interval)
{
    timer_.interval(interval);
    factor_ = double(senf::ClockService::in_nanoseconds(interval)) /
        double(senf::ClockService::in_nanoseconds(
                   senf::ClockService::seconds(1)));
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
