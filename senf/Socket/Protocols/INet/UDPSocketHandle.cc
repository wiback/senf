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
    int sock = ::socket(PF_INET,SOCK_DGRAM,0);
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
    int sock = ::socket(PF_INET6,SOCK_DGRAM,0);
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
