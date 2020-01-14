//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief UDPv4SocketHandle and UDPv6SocketHandle non-inline non-template implementation
 */

#include "UDPSocketHandle.hh"
//#include "UDPSocketHandle.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <senf/Utils/Exception.hh>

//#include "UDPSocketHandle.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::UDPv4SocketProtocol

prefix_ void senf::UDPv4SocketProtocol::init_client()
    const
{
    int sock = ::socket(PF_INET,SOCK_DGRAM | SOCK_CLOEXEC, 0);
    if (sock < 0)
        SENF_THROW_SYSTEM_EXCEPTION("could not open UDPv4Socket");
    fd(sock);
}

prefix_ void
senf::UDPv4SocketProtocol::init_client(INet4SocketAddress const & address)
    const
{
    init_client();
    reuseaddr(true);
    clientHandle().bind(address);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::UDPv6SocketProtocol::

prefix_ void senf::UDPv6SocketProtocol::init_client()
    const
{
    int sock = ::socket(PF_INET6,SOCK_DGRAM | SOCK_CLOEXEC, 0);
    if (sock < 0)
        SENF_THROW_SYSTEM_EXCEPTION("could not open UDPv6Socket");
    fd(sock);
}

prefix_ void
senf::UDPv6SocketProtocol::init_client(INet6SocketAddress const & address)
    const
{
    init_client();
    clientHandle().bind(address);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "UDPSocketHandle.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
