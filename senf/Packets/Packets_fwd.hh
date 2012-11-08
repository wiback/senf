// $Id$
//
// Copyright (C) 2012
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
//   Thorsten Horstmann <tho@berlios.de>

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
