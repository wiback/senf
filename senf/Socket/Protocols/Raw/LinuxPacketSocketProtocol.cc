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
    \brief LinuxPacketSocketProtocol non-inline non-template implementation */

#include "LinuxPacketSocketProtocol.hh"
//#include "LinuxPacketSocketProtocol.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/if.h>

//#include "LinuxPacketSocketProtocol.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    void do_mc(int fd, std::string const & interface, senf::MACAddress address, bool add)
    {
        struct ::packet_mreq mreq;
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

prefix_ void senf::LinuxPacketSocketProtocol::mcAdd(std::string const & interface,
                                         MACAddress const & address)
    const
{
    do_mc(fd(), interface, address, true);
}

prefix_ void senf::LinuxPacketSocketProtocol::mcDrop(std::string const & interface,
                                          MACAddress const & address)
    const
{
    do_mc(fd(), interface, address, false);
}

prefix_ void senf::LinuxPacketSocketProtocol::promisc(std::string const & interface, bool mode)
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

prefix_ unsigned senf::LinuxPacketSocketProtocol::rxQueueDropped()
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
//#include "LinuxPacketSocketProtocol.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
