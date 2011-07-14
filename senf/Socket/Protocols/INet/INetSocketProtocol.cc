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
