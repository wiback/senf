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


#include "RawINetSocketHandle.hh"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <senf/Utils/Exception.hh>

//#include "UDPSocketHandle.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::RawV4SocketProtocol

prefix_ void senf::RawV4SocketProtocol::init_client()
    const
{
    init_client(IPPROTO_RAW);
}

prefix_ void
senf::RawV4SocketProtocol::init_client(int const & protocol)
    const
{
    int sock = ::socket(PF_INET, SOCK_RAW, protocol);
    if (sock < 0)
        SENF_THROW_SYSTEM_EXCEPTION("could not open RawV4Socket");
    fd(sock);
}

prefix_ void
senf::RawV4SocketProtocol::init_client(int const & protocol, INet4SocketAddress const & address)
    const
{
    init_client(protocol);
    clientHandle().bind(address);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::RawV6SocketProtocol::

prefix_ void senf::RawV6SocketProtocol::init_client()
    const
{
    init_client(IPPROTO_RAW);
}

prefix_ void senf::RawV6SocketProtocol::init_client(int const & protocol)
    const
{
    int sock = ::socket(PF_INET6,SOCK_RAW,protocol);
    if (sock < 0)
        SENF_THROW_SYSTEM_EXCEPTION("could not open RawV6Socket");
    fd(sock);
}

prefix_ void
senf::RawV6SocketProtocol::init_client(int const & protocol, INet6SocketAddress const & address)
    const
{
    init_client(protocol);
    clientHandle().bind(address);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "RawINetSocketHandle.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
