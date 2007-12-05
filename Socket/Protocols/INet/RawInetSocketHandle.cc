// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum NETwork research (NET)
//     David Wagner <david.wagner@fokus.fraunhofer.de>
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

#include "RawInetSocketHandle.hh"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include "../../../Utils/Exception.hh"

//#include "UDPSocketHandle.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::UDPv4SocketProtocol

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
        throwErrno();
    body().fd(sock);
}

prefix_ void
senf::RawV4SocketProtocol::init_client(int const & protocol, INet4SocketAddress const & address)
    const
{
    init_client(protocol);
    bind(address);
}

prefix_ std::auto_ptr<senf::SocketProtocol> senf::RawV4SocketProtocol::clone()
    const
{
    return std::auto_ptr<SocketProtocol>(new RawV4SocketProtocol());
}

///////////////////////////////////////////////////////////////////////////
// senf::UDPv6SocketProtocol::

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
        throwErrno();
    body().fd(sock);
}

prefix_ void
senf::RawV6SocketProtocol::init_client(int const & protocol, INet6SocketAddress const & address)
    const
{
    init_client(protocol);
    bind(address);
}

prefix_ std::auto_ptr<senf::SocketProtocol> senf::RawV6SocketProtocol::clone()
    const
{
    return std::auto_ptr<SocketProtocol>(new RawV6SocketProtocol());
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
