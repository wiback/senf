// $Id$
//
// Copyright (C) 2013
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
    \brief MMapSocketProtocol non-inline non-template implementation */

#include "MMapSocketProtocol.hh"
//#include "MMapSocketProtocol.ih"

// Custom includes
#include <linux/if_packet.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>

//#include "MMapSocketProtocol.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PACKET_TX_HAS_OFF
#define PACKET_TX_HAS_OFF 19
#endif
#ifndef PACKET_QDISC_BYPASS
#define PACKET_QDISC_BYPASS	20
#endif

prefix_ void senf::MMapSocketProtocol::close()
{
    close_mmap();
    SocketProtocol::close();
}

prefix_ void senf::MMapSocketProtocol::terminate()
    const
{
    terminate_mmap();
    SocketProtocol::terminate();
}

prefix_ void senf::MMapSocketProtocol::init_mmap(unsigned frameSize, unsigned rxqlen,
                                                 unsigned txqlen, unsigned reserve=0)
    const
{
    ::memset(&qi_, 0, sizeof(qi_));
    qi_.frameSize = frameSize;

    int v = TPACKET_V2;
    ::socklen_t l = sizeof(int);
    if (getsockopt(fd(), SOL_PACKET, PACKET_HDRLEN, (char *)&v, &l) != 0)
        SENF_THROW_SYSTEM_EXCEPTION("::getsockopt(SOL_PACKET, PACKET_HDRLEN)");
    qi_.hdrlen = TPACKET_ALIGN(v);

    v = TPACKET_V2;
    if (setsockopt(fd(), SOL_PACKET, PACKET_VERSION, (char *)&v, sizeof(v)) != 0 )
        SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(SOL_PACKET, PACKET_VERSION)");

    unsigned size (0);

    struct ::tpacket_req req;
    ::memset(&req, 0, sizeof(req));

    if (rxqlen > 0) {
        qi_.reserveSize = 0;
        if (reserve > 0
            && setsockopt(fd(), SOL_PACKET, PACKET_RESERVE, (char *)&reserve, sizeof(reserve)) == 0)
            qi_.reserveSize = reserve;
        req.tp_frame_nr = rxqlen;
        req.tp_frame_size = frameSize;
        req.tp_block_size = req.tp_frame_nr * req.tp_frame_size;
        req.tp_block_nr = 1;
        if (setsockopt(fd(), SOL_PACKET, PACKET_RX_RING,
                       reinterpret_cast<char *>(&req), sizeof(req)) != 0 )
            SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(SOL_PACKET, PACKET_RX_RING");
        size += req.tp_block_size;
    }

    if (txqlen > 0) {
        req.tp_frame_nr = txqlen;
        req.tp_frame_size = frameSize;
        req.tp_block_size = req.tp_frame_nr * req.tp_frame_size;
        req.tp_block_nr = 1;
        if (setsockopt(fd(), SOL_PACKET, PACKET_TX_RING,
                       reinterpret_cast<char *>(&req), sizeof(req)) != 0 )
            SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(SOL_PACKET, PACKET_TX_RING");
        v = 1;
#ifdef SENF_ENABLE_TPACKET_OFFSET
        if (setsockopt(fd(), SOL_PACKET, PACKET_TX_HAS_OFF, (char *)&v, sizeof(v)) != 0)
            SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(SOL_PACKET, PACKET_TX_HAS_OFF");
#endif
        size += req.tp_block_size;

        v = 1;
        setsockopt(fd(), SOL_PACKET, PACKET_QDISC_BYPASS, (char*)&v, sizeof(v));
    }

    unsigned char * map (static_cast<unsigned char *>(
                    ::mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd(), 0)));
    if (map == MAP_FAILED)
        SENF_THROW_SYSTEM_EXCEPTION("::mmap()");

    qi_.map = map;

    if (rxqlen > 0) {
        qi_.rx.begin = qi_.rx.head = qi_.rx.tail = map;
        qi_.rx.end = map + frameSize * rxqlen;
        qi_.rx.idle = true;
        map = qi_.rx.end;
    }

    if (txqlen > 0) {
        qi_.tx.begin = qi_.tx.head = qi_.tx.tail = map;
        qi_.tx.end = map + frameSize * txqlen;
        qi_.tx.idle = true;
   }

    senf::FileHandleAccess::extraPtr(fh(), &qi_);
}

prefix_ void senf::MMapSocketProtocol::close_mmap()
    const
{
    if (! qi_.map)
        return;
    if (::munmap(qi_.map, (qi_.rx.end - qi_.rx.begin) + (qi_.tx.end - qi_.tx.begin)) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("::munmap");
    ::memset(&qi_, 0, sizeof(qi_));
}

prefix_ void senf::MMapSocketProtocol::terminate_mmap()
    const
{
    if (! qi_.map)
        return;
    ::munmap(qi_.map, (qi_.rx.end - qi_.rx.begin) + (qi_.tx.end - qi_.tx.begin));
    ::memset(&qi_, 0, sizeof(qi_));
}


prefix_ unsigned senf::MMapReadableSocketProtocol::available()
    const
{
    // Since dequeue() will skip outgoing packets it's impossible to return a correct
    // value here without race conditions. Thus we always return the frame size
    return static_cast<detail::QueueInfo *>(senf::FileHandleAccess::extraPtr(fh()))->frameSize;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "MMapSocketProtocol.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
