// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
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
//   Stefan Bund <g0dil@berlios.de>

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

