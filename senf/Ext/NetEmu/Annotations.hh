// $Id:$
//
// Copyright (C) 2008
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
//       Stefan Bund <g0dil@berlios.de>

/** \file
    \brief Annotations public header */

#ifndef HH_SENF_Ext_NetEmu_Annotations_
#define HH_SENF_Ext_NetEmu_Annotations_ 1

// Custom includes
#include <senf/Socket/Protocols/Raw/MACAddress.hh>
#include <senf/Socket/QueueReadWritePolicy.hh>
#include <senf/Scheduler/ClockService.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

/** \brief Predefined packet %annotations

     \see \ref packet_usage_annotation "Packet Annotation"
 */
namespace annotations {

    /** \brief Incoming or outgoing interface */
    struct Interface
    {
        mutable MACAddress value;
        // Very nice: senf::MACAddress is based on boost::array which has a trivial default
        // constructor and such can be used in a simple annotation

        Interface();
        Interface(MACAddress const & v);
        bool operator<(Interface const & other) const;
        bool operator==(Interface const & other) const;
    };

    /** \brief Output %Interface annotation instance as it's string representation
        \related Interface
     */
    std::ostream & operator<<(std::ostream & os, Interface const & annotation);

    std::size_t hash_value(Interface const & mac) SENF_NOEXCEPT;


    struct FrameRxFlags{
        unsigned frameCorrupt:1;
        unsigned frameRetransmitted:1;
        unsigned frameDuplicate:1;
        unsigned frameReordered:1;
        unsigned framePredecessorLost:8;
        unsigned frameLength:12;
    };

    /** \brief Packet quality */
    struct Quality
    {
        short snr;
        short rssi;
        short noise;
        FrameRxFlags flags;

        Quality();
        bool operator<(Quality const & other) const;
        void setLoss(unsigned loss);
    };

    /** \brief Output %Quality annotation instance as it's string representation
        \related Quality
     */
    std::ostream & operator<<(std::ostream & os, Quality const & annotation);

    /** \brief Incoming packet timestamp */
    struct Timestamp
    {
        std::uint32_t timestamp[2];

        Timestamp();
        bool operator<(Timestamp const & other) const;
        Timestamp &operator=(Timestamp const & other);

        void fromScheduler();
        void fromSocketProtocol(senf::DatagramSocketProtocol const & protocol);
        void fromWallClock();
        void fromQueueBuffer(senf::SocketQueueBuffer const & buffer);
        senf::ClockService::clock_type as_clock_type() const;
        std::uint32_t as_milli_seconds(std::uint32_t modulo = 0xffffffff) const;
    };

    /** \brief Output %Timestamp annotation instance as it's string representation
        \related Timestamp
     */
    std::ostream & operator<<(std::ostream & os, Timestamp const & annotation);

    /** \brief Wireless modulation */
    struct WirelessModulation
    {
        boost::uint32_t id;

        WirelessModulation();
        bool operator<(WirelessModulation const & other) const;
    };

    /** \brief Output %WirelessModulation annotation instance as it's string representation
        \related WirelessModulation
     */
    std::ostream & operator<<(std::ostream & os, WirelessModulation const & annotation);

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "Annotations.cci"
//#include "Annotations.ct"
//#include "Annotations.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
