// $Id:$
//
// Copyright (C) 2010
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
    \brief TokenBucketFilter non-inline template implementation  */

#include "TokenBucketFilter.hh"

// Custom includes
#include <senf/Utils/membind.hh>
#include <senf/Utils/Console/Variables.hh>
#include <senf/Utils/Console/ParsedCommand.hh>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::emu::TokenBucketFilter::TokenBucketFilter(unsigned _burst, unsigned _rate, ppi::QueueingAlgorithm::ptr qAlgorithm)
    : queueAlgo_(qAlgorithm.release()),
      lastToken_( ClockService::now()),
      timer_( "TokenBucketFilter::timer", membind(&TokenBucketFilter::onTimeout, this)),
      bucketLimit_( _burst), bucketLowThresh_(0), bucketSize_( _burst),
      rate_( _rate), bucketEmpty_(0)
{
    route( input, output).autoThrottling( false);
    if (rate_ == 0)
        input.onRequest( &TokenBucketFilter::byPass);
    else
        input.onRequest( &TokenBucketFilter::onRequest);
    input.throttlingDisc( ppi::ThrottlingDiscipline::NONE);

    namespace fty = console::factory;
    dir.add("queue", queueAlgo_->consoleDir());
    dir.add( "burst", fty::Command(
            SENF_MEMBINDFNP( void, TokenBucketFilter, burst, (unsigned)))
        .doc( "Set the bucket size/limit in bytes"));
    dir.add( "burst", fty::Command(
            SENF_MEMBINDFNP( unsigned, TokenBucketFilter, burst, () const))
        .doc( "Get the bucket size/limit in bytes"));
    dir.add( "bucketLowThresh", fty::Command(
            SENF_MEMBINDFNP( void, TokenBucketFilter, bucketLowThresh, (unsigned)))
        .doc( "Set the bucket low threshold in percent. Below this, we start dropping frames indicating an empty(ing) bucket early."));
    dir.add( "bucketLowThresh", fty::Command(
            SENF_MEMBINDFNP( unsigned, TokenBucketFilter, bucketLowThresh, () const))
        .doc( "Get the bucket low threshold in bytes."));
    dir.add( "bucketSize", fty::Variable( boost::cref(TokenBucketFilter::bucketSize_))
        .doc( "Get the current bucket size in bytes. 0 means empty."));
    dir.add( "bucketEmpty", fty::Variable( boost::cref(TokenBucketFilter::bucketEmpty_))
        .doc( "Get bucket-is-empty counter. An empty bucket means queueing."));
    dir.add( "rateLimit", fty::Command(
            SENF_MEMBINDFNP( void, TokenBucketFilter, rate, (unsigned)))
        .doc( "set the rate limit in bits per second"));
    dir.add( "rateLimit", fty::Command(
            SENF_MEMBINDFNP( unsigned, TokenBucketFilter, rate, () const))
        .doc( "get the rate limit in bits per second"));
    dir.add( "deviation", fty::Command( &TokenBucketFilter::timerDeviation, this)
        .doc( "statistic to timer duration and deviation"));
}

prefix_ unsigned senf::emu::TokenBucketFilter::burst()
    const
{
    return bucketLimit_;
}

prefix_ void senf::emu::TokenBucketFilter::burst(unsigned bytes)
{
    bucketLimit_ = bytes;
}

prefix_ unsigned senf::emu::TokenBucketFilter::bucketLowThresh()
    const
{
    return bucketLowThresh_;
}

prefix_ void senf::emu::TokenBucketFilter::bucketLowThresh(unsigned t)
{
    bucketLowThresh_ = (bucketLimit_ * std::min(100u, t)) / 100u;
}

prefix_ unsigned senf::emu::TokenBucketFilter::rate()
    const
{
    return rate_;
}
prefix_ void senf::emu::TokenBucketFilter::timerDeviation(std::ostream & out)
{
    out << "Timer deviation " << timerDeviation_.data() << " nano secs." << std::endl;
    timerDeviation_.clear();
}

prefix_ void senf::emu::TokenBucketFilter::rate(unsigned bits_per_second)
{
    rate_ = bits_per_second;

    queueAlgo_->clear();
    
    if (rate_ == 0u) {
        input.onRequest( &TokenBucketFilter::byPass);
    }  else {
        input.onRequest( &TokenBucketFilter::onRequest);
    }
}

prefix_ void senf::emu::TokenBucketFilter::onTimeout()
{
    // ClockService::int64_type delta (senf::ClockService::in_nanoseconds(ClockService::now() - timer_.timeout()));
    // timerDeviation_.accumulate(delta);
    fillBucket();
    unsigned pktSize;
    while ((pktSize = queueAlgo_->peek(bucketSize_)) > 0) {
        bucketSize_ -= pktSize;
        output(queueAlgo_->front());
        queueAlgo_->pop();
    }
    if (queueAlgo_->empty())
        input.onRequest( &TokenBucketFilter::onRequest);
    else
        setTimeout();
}

prefix_ void senf::emu::TokenBucketFilter::setTimeout()
{
    ClockService::clock_type now (scheduler::now());
    Packet::size_type packetSize (queueAlgo_->peek());
    ClockService::clock_type defer (ClockService::nanoseconds((packetSize - bucketSize_) * 8000000000ul / rate_));
    timer_.timeout( now + defer);
}

prefix_ void senf::emu::TokenBucketFilter::fillBucket()
{
    ClockService::clock_type now (scheduler::now());
    ClockService::int64_type diff (ClockService::in_nanoseconds(now - lastToken_));

    lastToken_ = now;
    bucketSize_ += (diff * rate_) / 8000000000ul;
}

prefix_ void senf::emu::TokenBucketFilter::fillBucketLimit()
{
    fillBucket();
    bucketSize_ = std::min(bucketSize_, bucketLimit_);
}


prefix_ void senf::emu::TokenBucketFilter::byPass()
{
    output(input());
}

prefix_ void senf::emu::TokenBucketFilter::onRequestQueueing()
{
    bucketEmpty_++;
    if (queueAlgo_->enqueue(input())) {
        setTimeout();
    }
}


prefix_ void senf::emu::TokenBucketFilter::onRequest()
{
    Packet const & packet (input());
    Packet::size_type packetSize (packet.size());

    fillBucketLimit();
    
    if (packetSize <= bucketSize_) {
        if (bucketSize_ < bucketLowThresh_ &&  bucketSize_ <= (std::uint32_t(rand()) % bucketLowThresh_)) {
            // drop packet early...indicating an empty(ing) bucket
            bucketEmpty_++;
            return;
        }
        bucketSize_ -= packetSize;
        output.write( packet);
        return;
    }
    
    bucketEmpty_++;
    if (queueAlgo_->enqueue( packet)) {
        input.onRequest( &TokenBucketFilter::onRequestQueueing);
        setTimeout();
    }
}

prefix_ senf::ppi::QueueingAlgorithm & senf::emu::TokenBucketFilter::qAlgorithm()
    const
{
    return *queueAlgo_;
}

prefix_ void senf::emu::TokenBucketFilter::qAlgorithm(ppi::QueueingAlgorithm::ptr qAlgorithm)
{
    queueAlgo_.reset(qAlgorithm.release());
    dir.add("queue", queueAlgo_->consoleDir());
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
