// $Id$
//
// Copyright (C) 2013
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

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

prefix_ void senf::MMapSocketProtocol::init_mmap(unsigned frameSize, unsigned rxqlen,
                                                 unsigned txqlen)
    const
{
    ::memset(&qi_, 0, sizeof(qi_));
    qi_.frameSize = frameSize;

    int v = TPACKET_V2;
    if (setsockopt(fd(), SOL_PACKET, PACKET_VERSION, (char *)&v, sizeof(v)) != 0 )
        SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(SOL_PACKET, PACKET_VERSION)");

    unsigned size (0);

    struct ::tpacket_req req;
    ::memset(&req, 0, sizeof(req));

    if (rxqlen > 0) {
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
        size += req.tp_block_size;
    }

    char * map (static_cast<char *>(
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
