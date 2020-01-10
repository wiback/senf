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
    \brief REDQueue public header */

#ifndef HH_SENF_Ext_NetEmu_Modules_REDQueue_
#define HH_SENF_Ext_NetEmu_Modules_REDQueue_ 1

// Custom includes
#include <queue>
#include <senf/PPI/QueueingAlgorithm.hh>

///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace emu {

    class REDQueue
        : public ppi::QueueingAlgorithm
    {
    public:
        static ppi::QueueingAlgorithm::ptr create();


        typedef std::pair<std::uint32_t,std::uint8_t> limit_t;

        REDQueue(std::uint32_t _limit = 16384, std::uint8_t lowThreshPrecentage = 25);

        void limit(std::uint32_t bytes, std::uint8_t lowThreshPrecentage = 25);
        limit_t limit() const;

        std::uint32_t dropped() const;
        void incrDropped();
        void resetDropped();

    private:
        unsigned v_peek(unsigned maxSize) const;
        senf::Packet const & v_front() const;

        void v_pop();
        senf::Packet v_dequeue();
        bool v_enqueue(senf::Packet const & packet, bool force);
        unsigned v_size() const;
        void v_clear();
        bool v_empty() const;

        std::queue<senf::Packet> queue_;
        std::uint32_t queueLimit_;
        std::uint32_t lowThresh_;
        std::uint8_t  lowThreshPrecentage_;
        std::uint32_t queueSize_;
        mutable std::uint32_t frontPktSize_;
        std::uint32_t packetsDroppedTotal_;
        std::uint32_t packetsDroppedLast_;
    };

}}
///////////////////////////////hh.e////////////////////////////////////////
#include "REDQueue.cci"
//#include "REDQueue.ct"
//#include "REDQueue.cti"
#endif
