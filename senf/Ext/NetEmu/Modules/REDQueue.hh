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
