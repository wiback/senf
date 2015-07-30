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
    \brief REDQueue non-inline template implementation  */

#include "REDQueue.hh"

// Custom includes
#include <senf/Scheduler/ClockService.hh>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::ppi::QueueingAlgorithm::ptr senf::emu::REDQueue::create()
{
    return ppi::QueueingAlgorithm::ptr(new REDQueue(4096, 0));
}

prefix_ senf::emu::REDQueue::REDQueue(boost::uint32_t _limit, boost::uint8_t lowThreshPrecentage)
    : queueLimit_ (0), lowThresh_ (0), lowThreshPrecentage_ (lowThreshPrecentage)
{
    clear();
    limit( _limit, lowThreshPrecentage_);

    namespace fty = console::factory;
    consoleDir().add( "limit", fty::Command(
            SENF_MEMBINDFNP( void, REDQueue, limit, (boost::uint32_t, boost::uint8_t)))
        .doc( "set the RED queue size bytes and low threshold in percent"));
    consoleDir().add( "limit", fty::Command(
            SENF_MEMBINDFNP( boost::uint32_t, REDQueue, limit, () const))
        .doc( "get the RED queue size in bytes"));
    consoleDir().add( "dropped", fty::Command( &REDQueue::dropped, this)
        .doc( "get the number of dropped packets"));
    consoleDir().add( "size", fty::Command( &REDQueue::size, this)
        .doc( "return the current RED queue size (usage) in bytes"));
}

prefix_ bool senf::emu::REDQueue::v_enqueue(senf::Packet const & packet)
{
    if (queueSize_ > lowThresh_) {
        if ( (boost::uint32_t(rand()) % (queueLimit_ - lowThresh_)) <= (queueSize_ - lowThresh_) ) {
            packetsDropped_++;
            return false;  // no additional packet added to queue
        }
    }

    queue_.push(packet);
    queueSize_ += packet.size();
    return true;
}

prefix_ senf::Packet senf::emu::REDQueue::v_dequeue()
{
    Packet tmp (queue_.front());
    queue_.pop();
    if (tmp) {
        queueSize_ -= tmp.size();
    }
    return tmp;
}

prefix_ void senf::emu::REDQueue::v_clear()
{
    queueSize_ = 0;
    packetsDropped_ = 0;
    while (! queue_.empty())
        queue_.pop();
}

prefix_ unsigned senf::emu::REDQueue::v_frontPacketSize()
    const
{
    return queue_.front().size();
}

prefix_ void senf::emu::REDQueue::limit(boost::uint32_t bytes, boost::uint8_t lowThreshPrecentage)
{
    if (lowThreshPrecentage > 100)
        lowThreshPrecentage = 100;

    queueLimit_ = bytes;
    lowThreshPrecentage_ = lowThreshPrecentage;

    lowThresh_ = (queueLimit_ * lowThreshPrecentage_) / 100;
    while (queueSize_ > queueLimit_) {
        queueSize_ -= queue_.front().size();
        queue_.pop();
    }
}

prefix_ boost::uint32_t senf::emu::REDQueue::limit()
    const
{
    return queueLimit_;
}

prefix_ boost::uint32_t senf::emu::REDQueue::dropped()
    const
{
    return packetsDropped_;
}

prefix_ void senf::emu::REDQueue::incrDropped()
{
    packetsDropped_++;
}

prefix_ void senf::emu::REDQueue::resetDropped()
{
    packetsDropped_ = 0;
}

prefix_ unsigned senf::emu::REDQueue::v_size()
    const
{
    return queueSize_;
}

prefix_ bool senf::emu::REDQueue::v_empty()
    const
{
    return queue_.empty();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
