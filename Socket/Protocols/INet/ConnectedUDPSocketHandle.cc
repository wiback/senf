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
    \brief ConnectedUDPv4SocketHandle and ConnectedUDPv6SocketHandle non-inline non-template implementation
 */

#include "ConnectedUDPSocketHandle.hh"
//#include "ConnectedUDPSocketHandle.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include "../../../Utils/Exception.hh"

//#include "ConnectedUDPSocketHandle.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::ConnectedUDPv4SocketProtocol

prefix_ void senf::ConnectedUDPv4SocketProtocol::init_client()
    const
{
    int sock = ::socket(PF_INET,SOCK_DGRAM,0);
    if (sock < 0)
        SENF_THROW_SYSTEM_EXCEPTION("::socket(PF_INET,SOCK_DGRAM,0) failed.");
    fd(sock);
}

prefix_ void
senf::ConnectedUDPv4SocketProtocol::init_client(INet4SocketAddress const & address)
    const
{
    init_client();
    clientHandle().connect(address);
}

///////////////////////////////////////////////////////////////////////////
// senf::ConnectedUDPv6SocketProtocol::

prefix_ void senf::ConnectedUDPv6SocketProtocol::init_client()
    const
{
    int sock = ::socket(PF_INET6,SOCK_DGRAM,0);
    if (sock < 0)
        SENF_THROW_SYSTEM_EXCEPTION("::socket(PF_INET6,SOCK_DGRAM,0) failed.");
    fd(sock);
}

prefix_ void
senf::ConnectedUDPv6SocketProtocol::init_client(INet6SocketAddress const & address)
    const
{
    init_client();
    clientHandle().connect(address);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "ConnectedUDPSocketHandle.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
