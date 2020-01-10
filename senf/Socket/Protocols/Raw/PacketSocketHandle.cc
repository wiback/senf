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
    \brief PacketSocketProtocol and PacketSocketHandle non-inline non-template implementation
 */

#include "PacketSocketHandle.hh"
//#include "PacketSocketHandle.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <errno.h>

//#include "PacketSocketHandle.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ void senf::PacketSocketProtocol::init_client(SocketType type, int protocol)
    const
{
    init_packetSocket(type, protocol);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "PacketSocketHandle.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
