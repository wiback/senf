// $Id: BufferingPolicy.cc 533 2007-11-23 17:34:30Z g0dil $
//
// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>
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
    \brief BufferingPolicy non-inline non-template implementation
 */

#include "NetdeviceController.hh"
//#include "NetdeviceController.ih"

// Custom includes
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/ether.h>
#include "../Utils/Exception.hh"

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::NetdeviceController::NetdeviceController(std::string const interface_name)
{
    sockfd_ = ::socket(PF_PACKET, SOCK_DGRAM, 0);
    if (sockfd_ < 0)
        throwErrno();
    interfacename_ = interface_name;
}

prefix_ senf::MACAddress senf::NetdeviceController::hardwareAddress()
{
    struct ifreq ifr;
    ::memset( &ifr, 0, sizeof(ifr));
    interfacename_.copy( ifr.ifr_name, IFNAMSIZ);
    if ( ::ioctl( sockfd_, SIOCGIFHWADDR , &ifr ) < 0 )
        throwErrno();
    return senf::MACAddress::from_string( 
            ether_ntoa( (struct ether_addr*) ifr.ifr_hwaddr.sa_data ) );
}

prefix_ senf::NetdeviceController::~NetdeviceController()
{
    close( sockfd_ );
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
