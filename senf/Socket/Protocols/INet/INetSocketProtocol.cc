// $Id$
//
// Copyright (C) 2006
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
    \brief INet[46]Protocol non-inline non-template implementation */

#include "INetSocketProtocol.hh"
//#include "INetSocketProtocol.ih"

// Custom includes
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <senf/Utils/Exception.hh>

//#include "INetSocketProtocol.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::INetSocketProtocol

prefix_ void senf::INetSocketProtocol::bindInterface(std::string const & iface)
    const
{
    if (::setsockopt(fd(), SOL_SOCKET, SO_BINDTODEVICE, iface.c_str(), iface.size()) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("::setsockopt(SO_BINDTODEVICE)");
}

prefix_ std::string senf::INetSocketProtocol::bindInterface()
{
    char iface[IFNAMSIZ];
    socklen_t size (sizeof(iface));
    ::memset(iface, 0, sizeof(iface));
    if (::getsockopt(fd(), SOL_SOCKET, SO_BINDTODEVICE, iface, &size) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("::getsockopt(SO_BINDTODEVICE)");
    iface[size < IFNAMSIZ ? size : IFNAMSIZ-1] = 0;
    return iface;
}

///////////////////////////////////////////////////////////////////////////
// senf::IPv4SocketProtocol

///////////////////////////////////////////////////////////////////////////
// senf::IPv6SocketProtocol

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "INetSocketProtocol.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
