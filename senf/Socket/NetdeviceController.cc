// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief NetdeviceController non-inline non-template implementation
 */

#include "NetdeviceController.hh"
//#include "NetdeviceController.ih"

// Custom includes
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <boost/weak_ptr.hpp>
#include <senf/Utils/Exception.hh>
#include <senf/Socket/Protocols/Raw/MACAddress.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

#define doIoctl(ifr, request, errorMsg)                                                         \
    if ( ::ioctl( sockfd_->fd, request, &ifr ) < 0 )                                            \
        SENF_THROW_SYSTEM_EXCEPTION("NetdeviceController: " errorMsg)


prefix_ senf::NetdeviceController::NetdeviceController(std::string const & interface_name)
    : sockfd_ (sockfd())
{
    struct ifreq ifr;
    ::memset( &ifr, 0, sizeof(ifr));
    interface_name.copy( ifr.ifr_name, IFNAMSIZ);
    doIoctl(ifr, SIOCGIFINDEX, "Could not discover the index of interface \"" + interface_name + "\"");
    ifindex_ = ifr.ifr_ifindex;
}

prefix_ senf::NetdeviceController::NetdeviceController(int interface_index)
    : sockfd_ (sockfd())
{
    ifindex_ = interface_index;
}

prefix_ std::string senf::NetdeviceController::interfaceName()
    const
{
    struct ifreq ifr;
    ifrName( ifr);
    return std::string( ifr.ifr_name);
}

prefix_ void senf::NetdeviceController::interfaceName(std::string const & newname)
{
    if (sizeof(newname) <= IFNAMSIZ) {
        struct ifreq ifr;
        ifrName(ifr);
        strncpy(ifr. ifr_newname, newname.c_str(), IFNAMSIZ);
        doIoctl(ifr, SIOCSIFNAME, "Could not change the interface name. Is the interface really down?");
    }
    return;
}

prefix_ senf::MACAddress senf::NetdeviceController::hardwareAddress()
    const
{
    struct ifreq ifr;
    ifrName( ifr);
    doIoctl( ifr, SIOCGIFHWADDR, "Could not discover hardwareAddress");
    return MACAddress::from_data( ifr.ifr_hwaddr.sa_data);
}

prefix_ void senf::NetdeviceController::hardwareAddress(MACAddress const & newAddress) {
    struct ifreq ifr;
    ifrName( ifr);
    ifr.ifr_hwaddr.sa_family = 1; // TODO: lookup named constant; PF_LOCAL ???
    std::copy(newAddress.begin(), newAddress.end(), ifr.ifr_hwaddr.sa_data);
    doIoctl(ifr, SIOCSIFHWADDR, "Could not change the interface MAC address. Is the interface really down?");
}

prefix_ int senf::NetdeviceController::mtu()
    const
{
    struct ifreq ifr;
    ifrName( ifr);
    doIoctl( ifr, SIOCGIFMTU, "Could not discover mtu");
    return ifr.ifr_mtu;
}

prefix_ void senf::NetdeviceController::mtu(int new_mtu)
{
    struct ifreq ifr;
    ifrName( ifr);
    ifr.ifr_mtu = new_mtu;
    doIoctl( ifr, SIOCSIFMTU, "Could not set mtu");
}

prefix_ int senf::NetdeviceController::txqueuelen()
    const
{
    struct ifreq ifr;
    ifrName( ifr);
    doIoctl( ifr, SIOCGIFTXQLEN, "Could not discover txqueuelen");
    return ifr.ifr_qlen;
}

prefix_ void senf::NetdeviceController::txqueuelen(int new_txqueuelen)
{
    struct ifreq ifr;
    ifrName( ifr);
    ifr.ifr_qlen = new_txqueuelen;
    doIoctl( ifr, SIOCSIFTXQLEN, "Could not set txqueuelen");
}


prefix_ bool senf::NetdeviceController::promisc()
    const
{
    struct ifreq ifr;
    ifrName( ifr);
    doIoctl( ifr, SIOCGIFFLAGS, "Could not discover promisc mode");
    return ifr.ifr_flags & IFF_PROMISC;
}

prefix_ void senf::NetdeviceController::promisc(bool mode)
{
    struct ifreq ifr;
    ifrName( ifr);
    doIoctl( ifr, SIOCGIFFLAGS, "Could not set promisc mode");
    if (mode)
        ifr.ifr_flags |= IFF_PROMISC;
    else
        ifr.ifr_flags &= ~IFF_PROMISC;
    doIoctl( ifr, SIOCSIFFLAGS, "Could not set promisc mode");
}

prefix_ bool senf::NetdeviceController::isUp()
    const
{
    struct ifreq ifr;
    ifrName(ifr);
    doIoctl(ifr, SIOCGIFFLAGS, "Could not discover interface status");
    return ifr.ifr_flags & IFF_UP;
}

prefix_ void senf::NetdeviceController::up()
{
    struct ifreq ifr;
    ifrName(ifr);
    doIoctl(ifr, SIOCGIFFLAGS, "Could not set interface status");
    ifr.ifr_flags |= IFF_UP;
    doIoctl(ifr, SIOCSIFFLAGS, "Could not set interface status");
}

prefix_ void senf::NetdeviceController::down()
{
    struct ifreq ifr;
    ifrName(ifr);
    doIoctl(ifr, SIOCGIFFLAGS, "Could not set interface status");
    ifr.ifr_flags &= ~IFF_UP;
    doIoctl(ifr, SIOCSIFFLAGS, "Could not set interface status");
}

prefix_ int senf::NetdeviceController::interfaceIndex()
    const
{
    return ifindex_;
}

prefix_ void senf::NetdeviceController::ifrName(ifreq & ifr)
    const
{
    ::memset( &ifr, 0, sizeof(ifr));
    ifr.ifr_ifindex = ifindex_;
    if ( ::ioctl( sockfd_->fd, SIOCGIFNAME, &ifr ) < 0 )
        SENF_THROW_SYSTEM_EXCEPTION("NetdeviceController")
        << " could not discover the name of the interface with index " << ifindex_ << ".";
}

#undef doIoctl

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::NetdeviceController::SockFd

prefix_ senf::NetdeviceController::SockFd::SockFd()
    : fd (::socket(PF_INET, SOCK_DGRAM, 0))
{
    if ( fd < 0)
        SENF_THROW_SYSTEM_EXCEPTION("Could not open socket for NetdeviceController.");
}

prefix_ senf::NetdeviceController::SockFd::~SockFd()
{
    ::close(fd);
}

prefix_ senf::NetdeviceController::SockFd::ptr senf::NetdeviceController::sockfd()
{
    static boost::weak_ptr<SockFd> sockfd;
    SockFd::ptr p (sockfd.lock());
    if (!p)
         sockfd = p = SockFd::ptr(new SockFd());
    return p;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "NetdeviceController.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
