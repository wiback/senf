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
    \brief TCPv4SocketHandle and TCPv6SocketHandle non-inline non-template implementation
 */

#include "TCPSocketHandle.hh"
//#include "TCPSocketHandle.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include "../../../Utils/Exception.hh"

//#include "TCPSocketHandle.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::TCPv4SocketProtocol

prefix_ void senf::TCPv4SocketProtocol::init_client()
    const
{
    int sock = ::socket(PF_INET,SOCK_STREAM,0);
    if (sock < 0)
        throwErrno();
    fd(sock);
}

prefix_ void
senf::TCPv4SocketProtocol::init_client(INet4SocketAddress const & address)
    const
{
    init_client();
    clientHandle().connect(address);
}

prefix_ void senf::TCPv4SocketProtocol::init_server()
    const
{
    int sock = ::socket(PF_INET,SOCK_STREAM,0);
    if (sock < 0)
        throwErrno();
    fd(sock);
}

prefix_ void senf::TCPv4SocketProtocol::init_server(INet4SocketAddress const & address,
                                                           unsigned backlog)
    const
{
    init_server();
    serverHandle().bind(address);
    reuseaddr(true);
    if (::listen(fd(),backlog) < 0)
        throwErrno();
}

///////////////////////////////////////////////////////////////////////////
// senf::TCPv6SocketProtocol::

prefix_ void senf::TCPv6SocketProtocol::init_client()
    const
{
    int sock = ::socket(PF_INET6,SOCK_STREAM,0);
    if (sock < 0)
        throwErrno();
    fd(sock);
}

prefix_ void
senf::TCPv6SocketProtocol::init_client(INet6SocketAddress const & address)
    const
{
    init_client();
    clientHandle().connect(address);
}

prefix_ void senf::TCPv6SocketProtocol::init_server()
    const
{
    int sock = ::socket(PF_INET6,SOCK_STREAM,0);
    if (sock < 0)
        throwErrno();
    fd(sock);
}

prefix_ void senf::TCPv6SocketProtocol::init_server(INet6SocketAddress const & address,
                                                    unsigned backlog)
    const
{
    init_server();
    serverHandle().bind(address);
    reuseaddr(true);
    if (::listen(fd(),backlog) < 0)
        throwErrno();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "TCPSocketHandle.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
