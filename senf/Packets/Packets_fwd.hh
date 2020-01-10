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
    \brief Packets_fwd public header */

#ifndef HH_SENF_Packets_Packets_fwd_
#define HH_SENF_Packets_Packets_fwd_ 1

// Custom includes

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
    template <class PacketType>
    class ConcretePacket;
}

#define SENF_PACKET_FWD_DECL( Pkt )                 \
    struct Pkt##Type;                               \
    typedef senf::ConcretePacket<Pkt##Type> Pkt;

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#endif
