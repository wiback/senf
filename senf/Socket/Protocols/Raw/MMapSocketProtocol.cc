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
    ::memset(&qi_, sizeof(qi_), 0);
    qi_.frameSize = frameSize;

    int v = TPACKET_V2;
    if (setsockopt(fd(), SOL_PACKET, PACKET_VERSION, (char *)&v, sizeof(v)) != 0 )
        SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(SOL_PACKET, PACKET_VERSION)");

    unsigned nrings (0);

    struct ::tpacket_req req;
    ::memset(&req, sizeof(req), 0);

    if (rxqlen > 0) {
        req.tp_frame_nr = rxqlen;
        req.tp_frame_size = frameSize;
        req.tp_block_size = req.tp_frame_nr * req.tp_frame_size;
        req.tp_block_nr = 1;
        int nrings (0);
        if (setsockopt(fd(), SOL_PACKET, PACKET_RX_RING,
                       reinterpret_cast<char *>(&req), sizeof(req)) != 0 )
            SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(SOL_PACKET, PACKET_RX_RING");
        ++ nrings;
    }

    if (txqlen > 0) {
        req.tp_frame_nr = txqlen;
        req.tp_frame_size = frameSize;
        req.tp_block_size = req.tp_frame_nr * req.tp_frame_size;
        req.tp_block_nr = 1;
        if (setsockopt(fd(), SOL_PACKET, PACKET_TX_RING,
                       reinterpret_cast<char *>(&req), sizeof(req)) != 0 )
            SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(SOL_PACKET, PACKET_TX_RING");
        ++ nrings;
    }

    char * map (static_cast<char *>(
                    ::mmap(NULL, req.tp_block_size * req.tp_block_nr * nrings,
                           PROT_READ|PROT_WRITE, MAP_SHARED, fd(), 0)));
    if (map == MAP_FAILED)
        SENF_THROW_SYSTEM_EXCEPTION("::mmap()");

    if (rxqlen > 0) {
        qi_.rx.begin = map;
        qi_.rx.end = map + frameSize * rxqlen;
        map = qi_.rx.end;
    }

    if (txqlen > 0) {
        qi_.tx.begin = map;
        qi_.tx.end = map + frameSize * txqlen;
   }

    senf::FileHandleAccess::extraPtr(fh(), &qi_);
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
