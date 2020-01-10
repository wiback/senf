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
    \brief PacketDelayer non-inline non-template implementation */

#include "PacketDelayer.hh"
//#include "PacketDelayer.ih"

// Custom includes
#include <senf/Utils/membind.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
prefix_ senf::emu::PacketDelayer::PacketDelayer()
    : timer_ ( "PacketDelayer::timer", membind(&PacketDelayer::timeout, this), ClockService::clock_type(0), false),
      variation_ (senf::ClockService::seconds(0))
{
    init();
    delay( senf::ClockService::seconds(0));
}

prefix_ senf::emu::PacketDelayer::PacketDelayer(ClockService::clock_type packetDelay)
    : timer_( "PacketDelayer::timer", membind(&PacketDelayer::timeout, this), ClockService::clock_type(0), false),
      variation_ (senf::ClockService::seconds(0))
{
    init();
    delay( packetDelay);
}

prefix_ void senf::emu::PacketDelayer::init()
{
    route( input, output);
    output.onThrottle( membind(&PacketDelayer::throttleNotification, this) );
    output.onUnthrottle( membind(&PacketDelayer::unthrottleNotification, this) );
    throttlingDisc( ppi::ThresholdThrottling(100, 99));
}

prefix_ void senf::emu::PacketDelayer::delay(ClockService::clock_type delay)
{
    delay_ = delay;
    enableDelay(delay != ClockService::seconds(0));
}

prefix_ senf::ClockService::clock_type senf::emu::PacketDelayer::delay()
    const
{
    return delay_;
}

prefix_ void senf::emu::PacketDelayer::variation(ClockService::clock_type variation)
{
    variation_ = variation;
    enableDelay(delay_ != ClockService::seconds(0) or variation != ClockService::seconds(0));
}

prefix_ senf::ClockService::clock_type senf::emu::PacketDelayer::variation()
    const
{
    return variation_;
}

prefix_ void senf::emu::PacketDelayer::enableDelay(bool flag)
{
    if (flag) {
        input.onRequest( &PacketDelayer::onRequest_delay);
        rndEngine_.reset( new std::mt19937(std::random_device()()));
        rndDist_.reset( new std::uniform_int_distribution<ClockService::int64_type>(
                ClockService::in_nanoseconds(-variation_), ClockService::in_nanoseconds(variation_)));
    } else {
        input.onRequest( &PacketDelayer::onRequest_noDelay);
        rndEngine_.release();
        rndDist_.release();
    }
}

prefix_ void senf::emu::PacketDelayer::onRequest_delay()
{
    queue_.push( ClockService::now() + delay_ + ClockService::nanoseconds((*rndDist_)(*rndEngine_)) );
    if (! timer_.enabled())
        timer_.timeout( queue_.front());
}

prefix_ void senf::emu::PacketDelayer::timeout()
{
    while (!queue_.empty() && queue_.front() <= ClockService::now()) {
        output.write( input.read());
        queue_.pop();
    }
    if (! queue_.empty())
        timer_.timeout( queue_.front());
}

prefix_ void senf::emu::PacketDelayer::throttleNotification()
{
    //tePullPacket.disable();
}

prefix_ void senf::emu::PacketDelayer::unthrottleNotification()
{
    //tePullPacket.enable();
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
