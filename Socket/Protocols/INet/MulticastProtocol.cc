// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
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
    \brief MulticastProtocol non-inline non-template implementation */

#include "MulticastProtocol.hh"
//#include "MulticastProtocol.ih"

// Custom includes
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h> // for if_nametoindex

//#include "MulticastProtocol.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::MulticastProtocol

prefix_ void senf::MulticastProtocol::broadcastEnabled(bool v)
{
    int ivalue (v);
    if (::setsockopt(fd(), SOL_SOCKET, SO_BROADCAST, &ivalue, sizeof(ivalue)) < 0)
        throwErrno("::setsockopt(SO_BROADCAST)");
}

prefix_ bool senf::MulticastProtocol::broadcastEnabled()
{
    int value (0);
    ::socklen_t len (sizeof(value));
    if (::getsockopt(fd(), SOL_SOCKET, SO_BROADCAST, &value, &len) < 0)
        throwErrno("::getsockopt(SO_BROADCAST)");
    return value;
}

prefix_ bool senf::MulticastProtocol::mcLoop()
    const
{
    int value;
    socklen_t len (sizeof(value));
    if (::getsockopt(fd(),SOL_IP,IP_MULTICAST_LOOP,&value,&len) < 0)
        throwErrno();
    return value;
}

prefix_ void senf::MulticastProtocol::mcLoop(bool value)
    const
{
    int ivalue (value);
    if (::setsockopt(fd(),SOL_IP,IP_MULTICAST_LOOP,&ivalue,sizeof(ivalue)) < 0)
        throwErrno();
}

prefix_ void senf::MulticastProtocol::mcIface(std::string const & iface)
    const
{
    struct ip_mreqn mreqn;
    ::memset(&mreqn,sizeof(mreqn),0);
    if (!iface.empty()) {
        mreqn.imr_ifindex = if_nametoindex(iface.c_str());
        if (mreqn.imr_ifindex == 0)
            throwErrno(EINVAL);
    }
    if (::setsockopt(fd(),SOL_IP,IP_MULTICAST_IF,&mreqn,sizeof(mreqn)) < 0)
        throwErrno();
}

prefix_ unsigned senf::MulticastProtocol::mcTTL()
    const
{
    int value;
    socklen_t len (sizeof(value));
    if (::getsockopt(fd(),SOL_IP,IP_MULTICAST_TTL,&value,&len) < 0)
        throwErrno();
    return value;
}

prefix_ void senf::MulticastProtocol::mcTTL(unsigned value)
    const
{
    if (::setsockopt(fd(),SOL_IP,IP_MULTICAST_TTL,&value,sizeof(value)) < 0)
        throwErrno();
}

///////////////////////////////////////////////////////////////////////////
// senf::INet4MulticastProtocol

prefix_ void senf::INet4MulticastProtocol::mcAddMembership(INet4Address const & mcAddr)
    const
{
    struct ip_mreqn mreqn;
    mreqn.imr_multiaddr.s_addr = mcAddr.inaddr();
    mreqn.imr_address.s_addr = htons(INADDR_ANY);
    mreqn.imr_ifindex = 0;
    if (::setsockopt(fd(),SOL_IP,IP_ADD_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
        throwErrno("::setsockopt(IP_ADD_MEMBERSHIP");
}

prefix_ void senf::INet4MulticastProtocol::mcAddMembership(INet4Address const & mcAddr,
                                                           INet4Address const & localAddr)
    const
{
    struct ip_mreqn mreqn;
    mreqn.imr_multiaddr.s_addr = mcAddr.inaddr();
    mreqn.imr_address.s_addr = localAddr.inaddr();
    mreqn.imr_ifindex = 0;
    if (::setsockopt(fd(),SOL_IP,IP_ADD_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
        throwErrno("::setsockopt(IP_ADD_MEMBERSHIP");
}

prefix_ void senf::INet4MulticastProtocol::mcAddMembership(INet4Address const & mcAddr,
                                                           std::string const & iface)
    const
{
    struct ip_mreqn mreqn;
    mreqn.imr_multiaddr.s_addr = mcAddr.inaddr();
    mreqn.imr_address.s_addr = htons(INADDR_ANY);
    mreqn.imr_ifindex = if_nametoindex(iface.c_str());
    if (mreqn.imr_ifindex == 0)
        throwErrno("::if_nametoindex()",ENOENT);
    if (::setsockopt(fd(),SOL_IP,IP_ADD_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
        throwErrno("::setsockopt(IP_ADD_MEMBERSHIP");
}

prefix_ void senf::INet4MulticastProtocol::mcDropMembership(INet4Address const & mcAddr)
    const
{
    struct ip_mreqn mreqn;
    mreqn.imr_multiaddr.s_addr = mcAddr.inaddr();
    mreqn.imr_address.s_addr = htons(INADDR_ANY);
    mreqn.imr_ifindex = 0;
    if (::setsockopt(fd(),SOL_IP,IP_DROP_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
        throwErrno();
}

prefix_ void senf::INet4MulticastProtocol::mcDropMembership(INet4Address const & mcAddr,
                                                            INet4Address const & localAddr)
    const
{
    struct ip_mreqn mreqn;
    mreqn.imr_multiaddr.s_addr = mcAddr.inaddr();
    mreqn.imr_address.s_addr = localAddr.inaddr();
    mreqn.imr_ifindex = 0;
    if (::setsockopt(fd(),SOL_IP,IP_DROP_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
        throwErrno();
}

prefix_ void senf::INet4MulticastProtocol::mcDropMembership(INet4Address const & mcAddr,
                                                            std::string const & iface)
    const
{
    struct ip_mreqn mreqn;
    mreqn.imr_multiaddr.s_addr = mcAddr.inaddr();
    mreqn.imr_address.s_addr = htons(INADDR_ANY);
    mreqn.imr_ifindex = if_nametoindex(iface.c_str());
    if (mreqn.imr_ifindex == 0)
        throwErrno("::if_nametoindex()",ENOENT);
    if (::setsockopt(fd(),SOL_IP,IP_DROP_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
        throwErrno();
}

///////////////////////////////////////////////////////////////////////////
// senf::INet6MulticastProtocol

prefix_ void senf::INet6MulticastProtocol::mcAddMembership(INet6Address const & mcAddr)
    const
{
    struct ipv6_mreq mreqn;
    std::copy(mcAddr.begin(), mcAddr.end(), mreqn.ipv6mr_multiaddr.s6_addr);
    mreqn.ipv6mr_interface = 0;
    if (::setsockopt(fd(),SOL_IP,IPV6_ADD_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
        throwErrno("::setsockopt(IPV6_ADD_MEMBERSHIP");
}

prefix_ void senf::INet6MulticastProtocol::mcAddMembership(INet6Address const & mcAddr,
                                                           std::string const & iface)
{
    struct ipv6_mreq mreqn;
    std::copy(mcAddr.begin(), mcAddr.end(), mreqn.ipv6mr_multiaddr.s6_addr);
    mreqn.ipv6mr_interface = if_nametoindex(iface.c_str());
    if (mreqn.ipv6mr_interface == 0)
        throwErrno("::if_nametoindex()",ENOENT);
    if (::setsockopt(fd(),SOL_IP,IPV6_ADD_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
        throwErrno("::setsockopt(IPV6_ADD_MEMBERSHIP");
}

prefix_ void senf::INet6MulticastProtocol::mcDropMembership(INet6Address const & mcAddr)
    const
{
    struct ipv6_mreq mreqn;
    std::copy(mcAddr.begin(), mcAddr.end(), mreqn.ipv6mr_multiaddr.s6_addr);
    mreqn.ipv6mr_interface = 0;
    if (::setsockopt(fd(),SOL_IP,IPV6_DROP_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
        throwErrno();
}

prefix_ void
senf::INet6MulticastProtocol::mcDropMembership(INet6Address const & mcAddr,
                                               std::string const & iface)
    const
{
    struct ipv6_mreq mreqn;
    std::copy(mcAddr.begin(), mcAddr.end(), mreqn.ipv6mr_multiaddr.s6_addr);
    mreqn.ipv6mr_interface = if_nametoindex(iface.c_str());
    if (mreqn.ipv6mr_interface == 0)
        throwErrno("::if_nametoindex()",ENOENT);
    if (::setsockopt(fd(),SOL_IP,IPV6_DROP_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
        throwErrno();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "MulticastProtocol.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
