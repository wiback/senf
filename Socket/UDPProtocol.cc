// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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
    \brief TCPProtocol non-inline non-template implementation
 */

#include "UDPProtocol.hh"
//#include "UDPProtocol.ih"

// Custom includes
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <linux/sockios.h> // for SIOCINQ / SIOCOUTQ
#include <net/if.h> // for if_nametoindex
#include "SocketHandle.hh"

//#include "UDPProtocol.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ unsigned senf::UDPProtocol::available()
    const
{
    int n;
    if (::ioctl(body().fd(),SIOCINQ,&n) < 0)
        throw senf::SystemException(errno);
    return n;
}

prefix_ bool senf::UDPProtocol::eof()
    const
{
    return false;
}

prefix_ bool senf::UDPProtocol::mcLoop()
    const
{
    int value;
    socklen_t len (sizeof(value));
    if (::getsockopt(body().fd(),SOL_IP,IP_MULTICAST_LOOP,&value,&len) < 0)
        throw SystemException(errno);
    return value;
}

prefix_ void senf::UDPProtocol::mcLoop(bool value)
    const
{
    int ivalue (value);
    if (::setsockopt(body().fd(),SOL_IP,IP_MULTICAST_LOOP,&ivalue,sizeof(ivalue)) < 0)
        throw SystemException(errno);
}

prefix_ void senf::UDPProtocol::mcAddMembership(INet4Address const & mcAddr)
    const
{
    struct ip_mreqn mreqn;
    mreqn.imr_multiaddr = reinterpret_cast<struct sockaddr_in const *>(mcAddr.sockaddr_p())->sin_addr;
    mreqn.imr_address.s_addr = htons(INADDR_ANY);
    mreqn.imr_ifindex = 0;
    if (::setsockopt(body().fd(),SOL_IP,IP_ADD_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
        throw SystemException(errno);
}

prefix_ void senf::UDPProtocol::mcAddMembership(INet4Address const & mcAddr,
                                                        INet4Address const & localAddr)
    const
{
    struct ip_mreqn mreqn;
    mreqn.imr_multiaddr = reinterpret_cast<struct sockaddr_in const *>(mcAddr.sockaddr_p())->sin_addr;
    mreqn.imr_address = reinterpret_cast<struct sockaddr_in const *>(localAddr.sockaddr_p())->sin_addr;
    mreqn.imr_ifindex = 0;
    if (::setsockopt(body().fd(),SOL_IP,IP_ADD_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
        throw SystemException(errno);
}

prefix_ void senf::UDPProtocol::mcDropMembership(INet4Address const & mcAddr)
    const
{
    struct ip_mreqn mreqn;
    mreqn.imr_multiaddr = reinterpret_cast<struct sockaddr_in const *>(mcAddr.sockaddr_p())->sin_addr;
    mreqn.imr_address.s_addr = htons(INADDR_ANY);
    mreqn.imr_ifindex = 0;
    if (::setsockopt(body().fd(),SOL_IP,IP_DROP_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
        throw SystemException(errno);
}

prefix_ void senf::UDPProtocol::mcDropMembership(INet4Address const & mcAddr,
                                                         INet4Address const & localAddr)
    const
{
    struct ip_mreqn mreqn;
    mreqn.imr_multiaddr = reinterpret_cast<struct sockaddr_in const *>(mcAddr.sockaddr_p())->sin_addr;
    mreqn.imr_address = reinterpret_cast<struct sockaddr_in const *>(localAddr.sockaddr_p())->sin_addr;
    mreqn.imr_ifindex = 0;
    if (::setsockopt(body().fd(),SOL_IP,IP_DROP_MEMBERSHIP,&mreqn,sizeof(mreqn)) < 0)
        throw SystemException(errno);
}

prefix_ void senf::UDPProtocol::mcIface(std::string iface)
    const
{
    struct ip_mreqn mreqn;
    ::memset(&mreqn,sizeof(mreqn),0);
    if (!iface.empty()) {
        mreqn.imr_ifindex = if_nametoindex(iface.c_str());
        if (mreqn.imr_ifindex == 0)
            throw SystemException(EINVAL);
    }
    if (::setsockopt(body().fd(),SOL_IP,IP_MULTICAST_IF,&mreqn,sizeof(mreqn)) < 0)
        throw SystemException(errno);
}

prefix_ unsigned senf::UDPProtocol::mcTTL()
    const
{
    int value;
    socklen_t len (sizeof(value));
    if (::getsockopt(body().fd(),SOL_IP,IP_MULTICAST_TTL,&value,&len) < 0)
        throw SystemException(errno);
    return value;
}

prefix_ void senf::UDPProtocol::mcTTL(unsigned value)
    const
{
    if (::setsockopt(body().fd(),SOL_IP,IP_MULTICAST_TTL,&value,sizeof(value)) < 0)
        throw SystemException(errno);
}


///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "UDPProtocol.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
