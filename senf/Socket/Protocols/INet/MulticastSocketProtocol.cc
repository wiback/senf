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
    \brief MulticastSocketProtocol non-inline non-template implementation */

#include "MulticastSocketProtocol.hh"
//#include "MulticastSocketProtocol.ih"

// Custom includes
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h> // for if_nametoindex
#include <senf/Utils/Exception.hh>

//#include "MulticastSocketProtocol.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::MulticastSocketProtocol

prefix_ void senf::MulticastSocketProtocol::broadcastEnabled(bool v)
    const
{
    int ivalue (v);
    if (::setsockopt(fd(), SOL_SOCKET, SO_BROADCAST, &ivalue, sizeof(ivalue)) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(SO_BROADCAST)");
}

prefix_ bool senf::MulticastSocketProtocol::broadcastEnabled()
    const
{
    int value (0);
    ::socklen_t len (sizeof(value));
    if (::getsockopt(fd(), SOL_SOCKET, SO_BROADCAST, &value, &len) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("::getsockopt(SO_BROADCAST)");
    return value;
}

prefix_ bool senf::MulticastSocketProtocol::mcLoop()
    const
{
    int value (0);
    socklen_t len (sizeof(value));
    if (::getsockopt(fd(),SOL_IP,IP_MULTICAST_LOOP,&value,&len) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
    return value;
}

prefix_ void senf::MulticastSocketProtocol::mcLoop(bool value)
    const
{
    int ivalue (value);
    if (::setsockopt(fd(),SOL_IP,IP_MULTICAST_LOOP,&ivalue,sizeof(ivalue)) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
}

prefix_ void senf::MulticastSocketProtocol::mcIface(std::string const & iface)
    const
{
    struct ip_mreqn mreqn;
    ::memset(&mreqn, 0, sizeof(mreqn));
    if (!iface.empty()) {
        mreqn.imr_ifindex = if_nametoindex(iface.c_str());
        if (mreqn.imr_ifindex == 0)
            throw SystemException(EINVAL SENF_EXC_DEBUGINFO);
    }
    if (::setsockopt(fd(),SOL_IP,IP_MULTICAST_IF,&mreqn,sizeof(mreqn)) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
}

prefix_ unsigned senf::MulticastSocketProtocol::mcTTL()
    const
{
    int value (0);
    socklen_t len (sizeof(value));
    if (::getsockopt(fd(),SOL_IP,IP_MULTICAST_TTL,&value,&len) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
    return value;
}

prefix_ void senf::MulticastSocketProtocol::mcTTL(unsigned value)
    const
{
    if (::setsockopt(fd(),SOL_IP,IP_MULTICAST_TTL,&value,sizeof(value)) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::INet4MulticastSocketProtocol

prefix_ void senf::INet4MulticastSocketProtocol::mcAddMembership(INet4Address const & mcAddr)
    const
{
    struct ip_mreqn mreqn;
    mreqn.imr_multiaddr.s_addr = mcAddr.inaddr();
    mreqn.imr_address.s_addr = htons(INADDR_ANY);
    mreqn.imr_ifindex = 0;
    if (::setsockopt(fd(),SOL_IP,IP_ADD_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(IP_ADD_MEMBERSHIP)");
}

prefix_ void senf::INet4MulticastSocketProtocol::mcAddMembership(INet4Address const & mcAddr,
                                                                 INet4Address const & localAddr)
    const
{
    struct ip_mreqn mreqn;
    mreqn.imr_multiaddr.s_addr = mcAddr.inaddr();
    mreqn.imr_address.s_addr = localAddr.inaddr();
    mreqn.imr_ifindex = 0;
    if (::setsockopt(fd(),SOL_IP,IP_ADD_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(IP_ADD_MEMBERSHIP");
}

prefix_ void senf::INet4MulticastSocketProtocol::mcAddMembership(INet4Address const & mcAddr,
                                                                 std::string const & iface)
    const
{
    struct ip_mreqn mreqn;
    mreqn.imr_multiaddr.s_addr = mcAddr.inaddr();
    mreqn.imr_address.s_addr = htons(INADDR_ANY);
    mreqn.imr_ifindex = if_nametoindex(iface.c_str());
    if (mreqn.imr_ifindex == 0)
        throw SystemException("::if_nametoindex()",ENOENT SENF_EXC_DEBUGINFO);
    if (::setsockopt(fd(),SOL_IP,IP_ADD_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(IP_ADD_MEMBERSHIP");
}

prefix_ void senf::INet4MulticastSocketProtocol::mcDropMembership(INet4Address const & mcAddr)
    const
{
    struct ip_mreqn mreqn;
    mreqn.imr_multiaddr.s_addr = mcAddr.inaddr();
    mreqn.imr_address.s_addr = htons(INADDR_ANY);
    mreqn.imr_ifindex = 0;
    if (::setsockopt(fd(),SOL_IP,IP_DROP_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
}

prefix_ void senf::INet4MulticastSocketProtocol::mcDropMembership(INet4Address const & mcAddr,
                                                                  INet4Address const & localAddr)
    const
{
    struct ip_mreqn mreqn;
    mreqn.imr_multiaddr.s_addr = mcAddr.inaddr();
    mreqn.imr_address.s_addr = localAddr.inaddr();
    mreqn.imr_ifindex = 0;
    if (::setsockopt(fd(),SOL_IP,IP_DROP_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
}

prefix_ void senf::INet4MulticastSocketProtocol::mcDropMembership(INet4Address const & mcAddr,
                                                                  std::string const & iface)
    const
{
    struct ip_mreqn mreqn;
    mreqn.imr_multiaddr.s_addr = mcAddr.inaddr();
    mreqn.imr_address.s_addr = htons(INADDR_ANY);
    mreqn.imr_ifindex = if_nametoindex(iface.c_str());
    if (mreqn.imr_ifindex == 0)
        throw SystemException("::if_nametoindex()",ENOENT SENF_EXC_DEBUGINFO);
    if (::setsockopt(fd(),SOL_IP,IP_DROP_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
}

namespace {
    void mc4SSMSourceRequest(int operation, int fd, senf::INet4Address const & group,
                              senf::INet4Address const & source, std::string const & iface)
    {
        struct group_source_req req;
        ::memset(&req, 0, sizeof(req));
        req.gsr_interface = if_nametoindex(iface.c_str());
        if (req.gsr_interface == 0)
            throw senf::SystemException("::if_nametoindex()", ENOENT SENF_EXC_DEBUGINFO);
        req.gsr_group.ss_family = AF_INET;
        reinterpret_cast<struct sockaddr_in&>(req.gsr_group).sin_addr.s_addr = group.inaddr();
        req.gsr_source.ss_family = AF_INET;
        reinterpret_cast<struct sockaddr_in&>(req.gsr_source).sin_addr.s_addr = source.inaddr();
        if (::setsockopt(fd, SOL_IP, MCAST_JOIN_SOURCE_GROUP, &req, sizeof(req)) < 0)
            SENF_THROW_SYSTEM_EXCEPTION("::setsockopt()");
    }
}

prefix_ void senf::INet4MulticastSocketProtocol::mcJoinSSMSource(INet4Address const & group,
                                                                 INet4Address const & source,
                                                                 std::string const & iface)
    const
{
    mc4SSMSourceRequest(MCAST_JOIN_SOURCE_GROUP, fd(), group, source, iface);
}

prefix_ void senf::INet4MulticastSocketProtocol::mcLeaveSSMSource(INet4Address const & group,
                                                                  INet4Address const & source,
                                                                  std::string const & iface)
    const
{
    mc4SSMSourceRequest(MCAST_LEAVE_SOURCE_GROUP, fd(), group, source, iface);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::INet6MulticastSocketProtocol

prefix_ void senf::INet6MulticastSocketProtocol::mcAddMembership(INet6Address const & mcAddr)
    const
{
    if (mcAddr.inet4Mapped()) {
        struct ip_mreqn mreqn;
        mreqn.imr_multiaddr.s_addr = mcAddr.inet4address().inaddr();
        mreqn.imr_address.s_addr = htons(INADDR_ANY);
        mreqn.imr_ifindex = 0;
        if (::setsockopt(fd(),SOL_IP,IP_ADD_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
            SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(IP_ADD_MEMBERSHIP)");
    }
    else {
        struct ipv6_mreq mreqn;
        std::copy(mcAddr.begin(), mcAddr.end(), mreqn.ipv6mr_multiaddr.s6_addr);
        mreqn.ipv6mr_interface = 0;
        if (::setsockopt(fd(),SOL_IPV6,IPV6_ADD_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
            SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(IPV6_ADD_MEMBERSHIP");
    }
}

prefix_ void senf::INet6MulticastSocketProtocol::mcAddMembership(INet6Address const & mcAddr,
                                                                 std::string const & iface)
    const
{
    if (mcAddr.inet4Mapped()) {
        struct ip_mreqn mreqn;
        mreqn.imr_multiaddr.s_addr = mcAddr.inet4address().inaddr();
        mreqn.imr_address.s_addr = htons(INADDR_ANY);
        mreqn.imr_ifindex = if_nametoindex(iface.c_str());
        if (mreqn.imr_ifindex == 0)
            throw SystemException("::if_nametoindex()",ENOENT SENF_EXC_DEBUGINFO);
        if (::setsockopt(fd(),SOL_IP,IP_ADD_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
            SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(IP_ADD_MEMBERSHIP");
    }
    else {
        struct ipv6_mreq mreqn;
        std::copy(mcAddr.begin(), mcAddr.end(), mreqn.ipv6mr_multiaddr.s6_addr);
        mreqn.ipv6mr_interface = if_nametoindex(iface.c_str());
        if (mreqn.ipv6mr_interface == 0)
            throw SystemException("::if_nametoindex()",ENOENT SENF_EXC_DEBUGINFO);
        if (::setsockopt(fd(),SOL_IPV6,IPV6_ADD_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
            SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(IPV6_ADD_MEMBERSHIP");
    }
}

prefix_ void senf::INet6MulticastSocketProtocol::mcDropMembership(INet6Address const & mcAddr)
    const
{
    if (mcAddr.inet4Mapped()) {
        struct ip_mreqn mreqn;
        mreqn.imr_multiaddr.s_addr = mcAddr.inet4address().inaddr();
        mreqn.imr_address.s_addr = htons(INADDR_ANY);
        mreqn.imr_ifindex = 0;
        if (::setsockopt(fd(),SOL_IP,IP_DROP_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
            SENF_THROW_SYSTEM_EXCEPTION("");
    }
    else {
        struct ipv6_mreq mreqn;
        std::copy(mcAddr.begin(), mcAddr.end(), mreqn.ipv6mr_multiaddr.s6_addr);
        mreqn.ipv6mr_interface = 0;
        if (::setsockopt(fd(),SOL_IPV6,IPV6_DROP_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
            SENF_THROW_SYSTEM_EXCEPTION("");
    }
}

prefix_ void
senf::INet6MulticastSocketProtocol::mcDropMembership(INet6Address const & mcAddr,
                                                     std::string const & iface)
    const
{
    if (mcAddr.inet4Mapped()) {
        struct ip_mreqn mreqn;
        mreqn.imr_multiaddr.s_addr = mcAddr.inet4address().inaddr();
        mreqn.imr_address.s_addr = htons(INADDR_ANY);
        mreqn.imr_ifindex = if_nametoindex(iface.c_str());
        if (mreqn.imr_ifindex == 0)
            throw SystemException("::if_nametoindex()",ENOENT SENF_EXC_DEBUGINFO);
        if (::setsockopt(fd(),SOL_IP,IP_DROP_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
            SENF_THROW_SYSTEM_EXCEPTION("");
    }
    else {
        struct ipv6_mreq mreqn;
        std::copy(mcAddr.begin(), mcAddr.end(), mreqn.ipv6mr_multiaddr.s6_addr);
        mreqn.ipv6mr_interface = if_nametoindex(iface.c_str());
        if (mreqn.ipv6mr_interface == 0)
            throw SystemException("::if_nametoindex()",ENOENT SENF_EXC_DEBUGINFO);
        if (::setsockopt(fd(),SOL_IPV6,IPV6_DROP_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
            SENF_THROW_SYSTEM_EXCEPTION("");
    }
}

namespace {

    void mc6SSMSourceRequest(int operation, int fd, senf::INet6Address const & group,
                             senf::INet6Address const & source, int ifacei)
    {
        struct group_source_req req;
        ::memset(&req, 0, sizeof(req));
        req.gsr_interface = ifacei;
        req.gsr_group.ss_family = AF_INET6;
        std::copy(group.begin(), group.end(),
                  reinterpret_cast<struct sockaddr_in6&>(req.gsr_group).sin6_addr.s6_addr);
        req.gsr_source.ss_family = AF_INET6;
        std::copy(source.begin(), source.end(),
                  reinterpret_cast<struct sockaddr_in6&>(req.gsr_source).sin6_addr.s6_addr);
        if (::setsockopt(fd, SOL_IPV6, MCAST_JOIN_SOURCE_GROUP, &req, sizeof(req)) < 0)
            SENF_THROW_SYSTEM_EXCEPTION("::setsockopt()");
    }

    void mc6SSMSourceRequest(int operation, int fd, senf::INet6Address const & group,
                             senf::INet6Address const & source, std::string const & iface)
    {
        int ifacei (0);
        if (! iface.empty()) {
            ifacei = if_nametoindex(iface.c_str());
            if (ifacei == 0)
                throw senf::SystemException("::if_nametoindex()", ENOENT SENF_EXC_DEBUGINFO);
        }
        mc6SSMSourceRequest(operation, fd, group, source, ifacei);
    }

}

prefix_ void senf::INet6MulticastSocketProtocol::mcJoinSSMSource(INet6Address const & group,
                                                                 INet6Address const & source,
                                                                 std::string const & iface)
    const
{
    mc6SSMSourceRequest(MCAST_JOIN_SOURCE_GROUP, fd(), group, source, iface);
}

prefix_ void senf::INet6MulticastSocketProtocol::mcJoinSSMSource(INet6Address const & group,
                                                                 INet6Address const & source,
                                                                 int ifacei)
    const
{
    mc6SSMSourceRequest(MCAST_JOIN_SOURCE_GROUP, fd(), group, source, ifacei);
}

prefix_ void senf::INet6MulticastSocketProtocol::mcLeaveSSMSource(INet6Address const & group,
                                                                  INet6Address const & source,
                                                                  std::string const & iface)
    const
{
    mc6SSMSourceRequest(MCAST_LEAVE_SOURCE_GROUP, fd(), group, source, iface);
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "MulticastSocketProtocol.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
