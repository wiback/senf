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
