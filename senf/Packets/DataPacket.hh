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
    \brief DataPacket public header */

#ifndef HH_SENF_Packets_DataPacket_
#define HH_SENF_Packets_DataPacket_ 1

// Custom includes
#include "PacketType.hh"
#include "Packet.hh"

//#include "DataPacket.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Generic payload-only packet

        \par Packet type (typedef):
            \ref DataPacket

        DataPacket is a simple generic packet with just a payload of uninterpreted data. This is the
        packet used whenever no more specific packet type can be found for a sub-packet (e.g. as the
        payload data of a UDP packet)

        \ingroup packet_module
     */
    struct DataPacketType : public PacketTypeBase
    {
        typedef ConcretePacket<DataPacketType> packet;
        static void dump(packet p, std::ostream & os);
    };

    /** \brief Generic payload-only packet typedef */
    typedef ConcretePacket<DataPacketType> DataPacket;
    SENF_PACKET_PREVENT_TEMPLATE_INSTANTIATION( DataPacket );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#endif
#if !defined(HH_SENF_Packets_Packets__decls_) && !defined(HH_SENF_Packets_DataPacket_impl_)
#define HH_SENF_Packets_DataPacket_impl_
//#include "DataPacket.cci"
//#include "DataPacket.ct"
//#include "DataPacket.cti"
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

