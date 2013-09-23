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
    int socktype = SOCK_RAW;
    if (type == DatagramSocket)
        socktype = SOCK_DGRAM;
    if (protocol == -1)
        protocol = ETH_P_ALL;
    int sock = ::socket(PF_PACKET, socktype, htons(protocol));
    if (sock < 0)
        SENF_THROW_SYSTEM_EXCEPTION("::socket(...) failed.");
    fd(sock);
}

prefix_ unsigned senf::PacketSocketProtocol::available()
    const
{
    if (! fh().readable())
        return 0;
    ssize_t l = ::recv(fd(),0,0,MSG_PEEK | MSG_TRUNC);
    if (l < 0)
        SENF_THROW_SYSTEM_EXCEPTION("::recv(socket_fd) failed.");
    return l;
}

prefix_ bool senf::PacketSocketProtocol::eof()
    const
{
    return false;
}

namespace {

    void do_mc(int fd, std::string const & interface, senf::MACAddress address, bool add)
    {
        struct packet_mreq mreq;
        ::memset(&mreq, 0, sizeof(mreq));
        mreq.mr_ifindex = ::if_nametoindex(interface.c_str());
        if (mreq.mr_ifindex == 0)
            throw senf::SystemException(EINVAL);
        mreq.mr_type = PACKET_MR_MULTICAST;
        mreq.mr_alen = 6;
        std::copy(address.begin(), address.end(), &mreq.mr_address[0]);
        if (::setsockopt(fd, SOL_PACKET,
                        add ? PACKET_ADD_MEMBERSHIP : PACKET_DROP_MEMBERSHIP,
                        &mreq, sizeof(mreq)) < 0)
            throw senf::SystemException();
    }
}

prefix_ void senf::PacketSocketProtocol::mcAdd(std::string const & interface,
                                         MACAddress const & address)
    const
{
    do_mc(fd(), interface, address, true);
}

prefix_ void senf::PacketSocketProtocol::mcDrop(std::string const & interface,
                                          MACAddress const & address)
    const
{
    do_mc(fd(), interface, address, false);
}

prefix_ void senf::PacketSocketProtocol::promisc(std::string const & interface, bool mode)
    const
{
    struct packet_mreq mreq;
    ::memset(&mreq, 0, sizeof(mreq));
    mreq.mr_ifindex = ::if_nametoindex(interface.c_str());
    if (mreq.mr_ifindex == 0)
        throw senf::SystemException(EINVAL);
    mreq.mr_type = PACKET_MR_PROMISC;
    if (::setsockopt(fd(), SOL_PACKET,
                     mode ? PACKET_ADD_MEMBERSHIP : PACKET_DROP_MEMBERSHIP,
                     &mreq, sizeof(mreq)) < 0)
        throw senf::SystemException();
}

prefix_ unsigned senf::PacketSocketProtocol::rxQueueDropped()
    const
{
    struct ::tpacket_stats stats;
    ::socklen_t len = sizeof(stats);
    if (::getsockopt(fd(), SOL_PACKET, PACKET_STATISTICS,
                     reinterpret_cast<char *>(&stats), &len) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("::getsocketopt(SOL_PACKET, PACKET_STATISTICS) failed");
    return stats.tp_drops;
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
