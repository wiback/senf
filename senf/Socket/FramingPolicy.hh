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
