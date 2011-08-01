// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief TCPv4SocketHandle and TCPv6SocketHandle non-inline non-template implementation
 */

#include "TCPSocketHandle.hh"
//#include "TCPSocketHandle.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <senf/Utils/Exception.hh>

//#include "TCPSocketHandle.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::TCPv4SocketProtocol

prefix_ void senf::TCPv4SocketProtocol::init_client()
    const
{
    int sock = ::socket(PF_INET,SOCK_STREAM,0);
    if (sock < 0)
        SENF_THROW_SYSTEM_EXCEPTION("could not open TCPv4Socket");
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
        SENF_THROW_SYSTEM_EXCEPTION("could not open TCPv4Socket");
    fd(sock);
}

prefix_ void senf::TCPv4SocketProtocol::init_server(INet4SocketAddress const & address,
                                                           unsigned backlog)
    const
{
    init_server();
    reuseaddr(true);
    serverHandle().bind(address);
    if (::listen(fd(),backlog) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("could not listen on TCPv4Socket");
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::TCPv6SocketProtocol::

prefix_ void senf::TCPv6SocketProtocol::init_client()
    const
{
    int sock = ::socket(PF_INET6,SOCK_STREAM,0);
    if (sock < 0)
        SENF_THROW_SYSTEM_EXCEPTION("could not open TCPv6Socket");
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
        SENF_THROW_SYSTEM_EXCEPTION("could not open TCPv6Socket");
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
        SENF_THROW_SYSTEM_EXCEPTION("could not listen on TCPv6Socket");
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
