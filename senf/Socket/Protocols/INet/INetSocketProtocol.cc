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
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::IPv4SocketProtocol

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::IPv6SocketProtocol

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
