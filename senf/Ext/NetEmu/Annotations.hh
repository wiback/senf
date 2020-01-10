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

    std::size_t hash_value(Interface const & mac) noexcept;

    struct FrameRxFlags{
        unsigned frameCorrupt:1;
        unsigned frameRetransmitted:1;
        unsigned frameDuplicate:1;
        unsigned frameReordered:1;
        unsigned frameAggregated:1;
        unsigned framePredecessorLost:8;
        unsigned frameLength:12;
    };

    /** \brief Packet quality */
    struct Quality
    {
        mutable short snr;
        mutable short rssi;
        mutable short noise;
        mutable unsigned short airTime;
        mutable FrameRxFlags flags;

        Quality();
//        bool operator<(Quality const & other) const;
        void setLoss(unsigned loss);
    };

    /** \brief Output %Quality annotation instance as it's string representation
        \related Quality
     */
    std::ostream & operator<<(std::ostream & os, Quality const & annotation);

    /** \brief Incoming packet timestamp */
    struct Timestamp
    {
        mutable std::uint32_t timestamp[2];

        Timestamp();
        bool operator<(Timestamp const & other) const;
        Timestamp &operator=(Timestamp const & other);

        void fromScheduler() const;
        void fromSocketProtocol(senf::DatagramSocketProtocol const & protocol) const;
        void fromWallClock() const;
        void fromQueueBuffer(senf::SocketQueueBuffer const & buffer) const;
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
        mutable boost::uint32_t id;

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
