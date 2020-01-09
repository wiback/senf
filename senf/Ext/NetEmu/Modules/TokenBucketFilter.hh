// $Id:$
//
// Copyright (C) 2010
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fit.fraunhofer.de/license.html
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
    \brief TokenBucketFilter public header */

#ifndef HH_SENF_Ext_NetEmu_Modules_TokenBucketFilter_
#define HH_SENF_Ext_NetEmu_Modules_TokenBucketFilter_ 1

// Custom includes
#include <iostream>
#include <senf/PPI/Module.hh>
#include <senf/Scheduler/TimerEvent.hh>
#include <senf/Utils/StatisticAccumulator.hh>
#include "REDQueue.hh"

///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace emu {

    class TokenBucketFilter
        : public ppi::module::Module
    {
        SENF_PPI_MODULE(TokenBucketFilter);

    public:
        console::ScopedDirectory<> dir;
        ppi::connector::PassiveInput<> input;
        ppi::connector::ActiveOutput<> output;

        TokenBucketFilter(unsigned _burst, unsigned _rate, ppi::QueueingAlgorithm::ptr qAlgorithm);

        void burst(unsigned bytes);           ///< Set size of the bucket, in bytes.
                                              /**< Set the maximum amount of bytes that tokens can be
                                                   available for instantaneously. */
        unsigned burst() const;               ///< Get size of the bucket, in bytes.
        void bucketLowThresh(unsigned percent);  ///< Set size of the bucket low thresh, in percent.
        unsigned bucketLowThresh() const;        ///< Get size of the bucket low thresh, in bytes.
        void rate(unsigned bits_per_second);  ///< Set rate of new tokens, in bits per second.
        unsigned rate() const;                ///< Get rate of new tokens, in bits per second.

        ppi::QueueingAlgorithm & qAlgorithm() const;
        void qAlgorithm(ppi::QueueingAlgorithm::ptr qAlgorithm);

        CyclicTimestamp const & lastToken() const;
        
    private:
        boost::scoped_ptr<ppi::QueueingAlgorithm> queueAlgo_;
        CyclicTimestamp const & now_;
        ClockService::clock_type const & schedulerNow_;
        CyclicTimestamp lastToken_;
        scheduler::TimerEvent timer_;
        unsigned bucketLimit_;
        unsigned bucketLowThresh_;
        unsigned bucketSize_;
        unsigned rate_;
        unsigned bucketEmpty_;

        void onTimeout();
        void onRequest();
        void onRequestQueueing();
        void byPass();
        void fillBucket();
        void fillBucketLimit();
        void setTimeout();
        bool queuePacket(Packet const & packet, Packet::size_type packetSize);
    };

}}
///////////////////////////////hh.e////////////////////////////////////////
#include "TokenBucketFilter.cci"
//#include "TokenBucketFilter.ct"
//#include "TokenBucketFilter.cti"
#endif
