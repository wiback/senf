//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
    int sock = ::socket(PF_INET,SOCK_STREAM | SOCK_CLOEXEC, 0);
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
    int sock = ::socket(PF_INET,SOCK_STREAM | SOCK_CLOEXEC, 0);
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
    int sock = ::socket(PF_INET6,SOCK_STREAM | SOCK_CLOEXEC, 0);
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
    int sock = ::socket(PF_INET6,SOCK_STREAM | SOCK_CLOEXEC, 0);
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
