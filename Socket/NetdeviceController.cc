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
#include "../Utils/Exception.hh"

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::NetdeviceController::NetdeviceController(std::string const & interface_name)
{
    openSocket();
    struct ifreq ifr;
    ::memset( &ifr, 0, sizeof(ifr));
    interface_name.copy( ifr.ifr_name, IFNAMSIZ);
    doIoctl( ifr, SIOCGIFINDEX);
    ifindex_ = ifr.ifr_ifindex;
}

prefix_ senf::NetdeviceController::NetdeviceController(int interface_index)
{
    openSocket();
    ifindex_ = interface_index;
}

prefix_ std::string senf::NetdeviceController::interfaceName()
{
    struct ifreq ifr;
    ifrName( ifr);
    return std::string( ifr.ifr_name);
}

prefix_ senf::MACAddress senf::NetdeviceController::hardwareAddress()
{
    struct ifreq ifr;
    ifrName( ifr);
    doIoctl( ifr, SIOCGIFHWADDR);
    return senf::MACAddress::from_data( ifr.ifr_hwaddr.sa_data);
}

prefix_ int senf::NetdeviceController::mtu()
{
    struct ifreq ifr;
    ifrName( ifr);
    doIoctl( ifr, SIOCGIFMTU);
    return ifr.ifr_mtu;
}

prefix_ void senf::NetdeviceController::mtu(int new_mtu)
{
    struct ifreq ifr;
    ifrName( ifr);
    ifr.ifr_mtu = new_mtu;
    doIoctl( ifr, SIOCSIFMTU);
}

prefix_ int senf::NetdeviceController::interfaceIndex()
{
    return ifindex_;
}

prefix_ senf::NetdeviceController::~NetdeviceController()
{
    close( sockfd_);
}

prefix_ void senf::NetdeviceController::openSocket()
{
    sockfd_ = ::socket( PF_INET, SOCK_DGRAM, 0);
    if ( sockfd_ < 0)
        throwErrno();
}

prefix_ void senf::NetdeviceController::ifrName(ifreq& ifr)
{
    ::memset( &ifr, 0, sizeof(ifr));
    ifr.ifr_ifindex = ifindex_;
    if ( ::ioctl( sockfd_, SIOCGIFNAME, &ifr ) < 0 )
        throwErrno();
}


prefix_ void senf::NetdeviceController::doIoctl(ifreq& ifr, int request)
{
    if ( ::ioctl( sockfd_, request, &ifr ) < 0 )
        throwErrno();
}

///////////////////////////////cc.e////////////////////////////////////////
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
