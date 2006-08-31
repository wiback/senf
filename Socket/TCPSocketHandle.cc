// $Id$
//
// Copyright (C) 2006 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

// Definition of non-inline non-template functions

#include "TCPSocketHandle.hh"
//#include "TCPSocketHandle.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include "Utils/Exception.hh"

//#include "TCPSocketHandle.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ bool satcom::lib::TCPv4SocketProtocol::reuseraddr()
    const
{
    int value;
    socklen_t len (sizeof(value));
    if (::getsockopt(body().fd(),SOL_SOCKET,SO_REUSEADDR,&value,&len) < 0)
        throw SystemException(errno);
    return value;
}

prefix_ void satcom::lib::TCPv4SocketProtocol::reuseaddr(bool value)
    const
{
    int ivalue (value);
    if (::setsockopt(body().fd(),SOL_SOCKET,SO_REUSEADDR,&ivalue,sizeof(ivalue)) < 0)
        throw SystemException(errno);
}

prefix_ void satcom::lib::TCPv4SocketProtocol::init_client()
    const
{
    int sock = ::socket(PF_INET,SOCK_STREAM,0);
    if (sock < 0)
        throw SystemException(errno);
    body().fd(sock);
}

prefix_ void
satcom::lib::TCPv4SocketProtocol::init_client(INet4Address const & address)
    const
{
    init_client();
    connect(address);
}

prefix_ void satcom::lib::TCPv4SocketProtocol::init_server()
    const
{
    int sock = ::socket(PF_INET,SOCK_STREAM,0);
    if (sock < 0)
        throw SystemException(errno);
    body().fd(sock);
}

prefix_ void
satcom::lib::TCPv4SocketProtocol::init_server(INet4Address const & address)
    const
{
    init_server();
    bind(address);
    reuseaddr(true);
    if (::listen(body().fd(),1) < 0)
        throw SystemException(errno);
}

prefix_ void satcom::lib::TCPv4SocketProtocol::connect(INet4Address const & address)
    const
{
    if (::connect(body().fd(),address.sockaddr_p(), address.sockaddr_len()) < 0)
        throw SystemException(errno);
}

prefix_ void satcom::lib::TCPv4SocketProtocol::bind(INet4Address const & address)
    const
{
    if (::bind(body().fd(),address.sockaddr_p(), address.sockaddr_len()) < 0)
        throw SystemException(errno);
}

prefix_ std::auto_ptr<satcom::lib::SocketProtocol> satcom::lib::TCPv4SocketProtocol::clone()
    const
{
    return std::auto_ptr<SocketProtocol>(new TCPv4SocketProtocol());
}

prefix_ unsigned satcom::lib::TCPv4SocketProtocol::available()
    const
{
    int n;
    if (::ioctl(body().fd(),FIONREAD,&n) < 0)
        throw SystemException(errno);
    return n;
}

prefix_ bool satcom::lib::TCPv4SocketProtocol::eof()
    const
{
    return body().readable() && available()==0;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "TCPSocketHandle.mpp"


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
