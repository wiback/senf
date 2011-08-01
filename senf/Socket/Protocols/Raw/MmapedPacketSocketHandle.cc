// $Id$
//
// Copyright (C) 2011
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
    \brief MmapedPacketSocketProtocol and MmapedPacketSocketHandle non-inline non-template implementation
 */

#include "MmapedPacketSocketHandle.hh"
//#include "MmapedPacketSocketHandle.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <errno.h>

//#include "MmapedPacketSocketHandle.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ void senf::MmapedPacketSocketProtocol::init_client(SocketType type, int protocol)
{
    /* Setup req */
    req_.tp_block_size = 4096;
    req_.tp_frame_size = 2048;
    req_.tp_block_nr = 32;
    req_.tp_frame_nr = 2*32;
    /* Setup socket */
    int socktype = (type == RawSocket ? SOCK_RAW : SOCK_DGRAM);
    if (protocol == -1)
        protocol = ETH_P_ALL;
    int sock = ::socket(PF_PACKET, socktype, htons(protocol));
    if (sock < 0)
        SENF_THROW_SYSTEM_EXCEPTION("::socket(...) failed.");
    if ( (setsockopt(sock, SOL_PACKET, PACKET_RX_RING, reinterpret_cast<char *>(&req_), sizeof(req_))) != 0 )
        SENF_THROW_SYSTEM_EXCEPTION("setsockopt(PACKET_RX_RING) failed.");
    /* Setup mmap */
    map_ = static_cast<char *>(
            mmap( NULL, req_.tp_block_size * req_.tp_block_nr, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_SHARED, sock, 0));
    if ( map_ == MAP_FAILED )
        SENF_THROW_SYSTEM_EXCEPTION("mmap failed.");
    /* Setup our ringbuffer */
    ring_.resize( req_.tp_frame_nr);
    for (unsigned i=0; i<req_.tp_frame_nr; i++) {
        ring_[i].iov_base = (void *) (&map_[i*req_.tp_frame_size]);
        ring_[i].iov_len = req_.tp_frame_size;
    }
    fd( sock);
}

prefix_ unsigned senf::MmapedPacketSocketProtocol::available()
    const
{

}

prefix_ bool senf::MmapedPacketSocketProtocol::eof()
    const
{
    return false;
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
