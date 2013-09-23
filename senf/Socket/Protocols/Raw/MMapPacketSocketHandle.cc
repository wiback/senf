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
    \brief MMapPacketSocketHandle non-inline non-template implementation */

#include "MMapPacketSocketHandle.hh"
//#include "MMapPacketSocketHandle.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <errno.h>

//#include "MMapPacketSocketHandle.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ void senf::ConnectedMMapPacketSocketProtocol::init_client(std::string iface,
                                                                  unsigned rxqlen,
                                                                  unsigned txqlen,
                                                                  unsigned frameSize,
                                                                  SocketType type, int protocol)
    const
{
    int socktype = SOCK_RAW;
    if (type == DatagramSocket)
        socktype = SOCK_DGRAM;
    if (protocol == -1)
        protocol = ETH_P_ALL;
    int sock = ::socket(PF_PACKET, socktype, htons(protocol));
    if (sock < 0)
        SENF_THROW_SYSTEM_EXCEPTION("::socket(PF_PACKET) failed.");
    fd(sock);
    clientHandle().bind(LLSocketAddress(iface));
    init_mmap(frameSize, rxqlen, txqlen);
}

prefix_ unsigned senf::ConnectedMMapPacketSocketProtocol::available()
    const
{
    // Since dequeue() will skip outgoing packets it's impossible to return a correct
    // value here without race conditions. Thus we always return the frame size
    return qi_.frameSize;
}

prefix_ bool senf::ConnectedMMapPacketSocketProtocol::eof()
    const
{
    return false;
}

prefix_ void senf::ConnectedMMapPacketSocketProtocol::close()
{
    close_mmap();
    SocketProtocol::close();
}

prefix_ void senf::ConnectedMMapPacketSocketProtocol::terminate()
    const
{
    terminate_mmap();
    SocketProtocol::terminate();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "MMapPacketSocketHandle.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
