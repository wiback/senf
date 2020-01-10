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
    \brief ConnectedUDPv4SocketHandle and ConnectedUDPv6SocketHandle non-inline non-template implementation
 */

#include "ConnectedUDPSocketHandle.hh"
//#include "ConnectedUDPSocketHandle.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <senf/Utils/Exception.hh>

//#include "ConnectedUDPSocketHandle.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
