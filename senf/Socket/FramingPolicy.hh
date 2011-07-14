// $Id$
//
// Copyright (C) 2006
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
    \brief FramingPolicy public header
 */

#ifndef HH_SENF_Socket_FramingPolicy_
#define HH_SENF_Socket_FramingPolicy_ 1

// Custom includes
#include "SocketPolicy.hh"

//#include "FramingPolicy.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /// \addtogroup policy_impl_group
    //\{

    /** \brief FramingPolicy for stream oriented sockets

        This policy does not explicitly modify the SocketHAndle
        API. It however affects the semantics of the read and write
        operations. On a stream oriented socket, read() and write()
        operations may be combined, the boundary between separate
        write() calls will be lost on the receiving side.
     */
    struct StreamFramingPolicy : public FramingPolicyBase
    {};

    /** \brief FramingPolicy for datagram oriented sockets

        This policy does not explicitly modify the SocketHAndle
        API. It however affects the semantics of the read and write
        operations. On a datagram socket, each read() or write() call
        we read or write a single datagram. Datagram boundaries are
        kept intact across the network.
     */
    struct DatagramFramingPolicy : public FramingPolicyBase
    {};

    //\}

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "FramingPolicy.cci"
//#include "FramingPolicy.ct"
//#include "FramingPolicy.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
