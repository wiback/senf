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
    \brief REDQueue non-inline template implementation  */

#include "REDQueue.hh"

// Custom includes
#include <senf/Scheduler/ClockService.hh>
#include <senf/Utils/Console/Console.hh>
#include <senf/Utils/Console/Variables.hh>
#include <senf/Utils/Console/STLSupport.hh>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::ppi::QueueingAlgorithm::ptr senf::emu::REDQueue::create()
{
    return ppi::QueueingAlgorithm::ptr(new REDQueue(16384, 25));
}

prefix_ senf::emu::REDQueue::REDQueue(boost::uint32_t _limit, boost::uint8_t lowThreshPrecentage)
    : queueLimit_ (0), lowThresh_ (0), lowThreshPrecentage_ (lowThreshPrecentage), frontPktSize_(0)
{
    clear();
    limit( _limit, lowThreshPrecentage_);

    namespace fty = console::factory;
    consoleDir().add( "limit", fty::Command(
            SENF_MEMBINDFNP( void, REDQueue, limit, (boost::uint32_t, boost::uint8_t)))
        .doc( "set the RED queue size bytes and low threshold in percent"));
    consoleDir().add( "limit", fty::Command(
            SENF_MEMBINDFNP( limit_t, REDQueue, limit, () const))
        .doc( "get the RED queue size in bytes and low threshold in percent"));
    consoleDir().add( "dropped", fty::Command( &REDQueue::dropped, this)
        .doc( "get the number of dropped packets since the last call."));
    consoleDir().add( "size", fty::Command( &REDQueue::size, this)
        .doc( "return the current RED queue size (usage) in bytes"));
    consoleDir().add( "droppedTotal", fty::Variable( boost::cref(REDQueue::packetsDroppedTotal_))
        .doc( "Get the total number of dropped packets."));
}


prefix_ void senf::emu::REDQueue::v_clear()
{
    queueSize_ = 0;
    packetsDroppedLast_ = 0;
    packetsDroppedTotal_ = 0;
    while (!queue_.empty())
        queue_.pop();
    frontPktSize_ = 0;
}


prefix_ void senf::emu::REDQueue::limit(boost::uint32_t bytes, boost::uint8_t lowThreshPrecentage)
{
    if (lowThreshPrecentage > 100)
        lowThreshPrecentage = 100;

    // 2GByte limit
    if (bytes > 0x7fffffff)
        bytes = 0x7fffffff;

    queueLimit_ = bytes;
    lowThreshPrecentage_ = lowThreshPrecentage;

    lowThresh_ = (queueLimit_ * lowThreshPrecentage_) / 100;

    clear();
}

prefix_ senf::emu::REDQueue::limit_t senf::emu::REDQueue::limit()
    const
{
    return std::make_pair(queueLimit_, lowThreshPrecentage_);
}

prefix_ boost::uint32_t senf::emu::REDQueue::dropped()
    const
{
    return packetsDroppedTotal_ - packetsDroppedLast_;
}

prefix_ void senf::emu::REDQueue::resetDropped()
{
    packetsDroppedLast_ = packetsDroppedTotal_;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
