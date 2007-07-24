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

/** \file
    \brief INet[46]Protocol non-inline non-template implementation */

#include "INetProtocol.hh"
//#include "INetProtocol.ih"

// Custom includes
#include <sys/socket.h>
#include <netinet/in.h>
#include "Utils/Exception.hh"

//#include "INetProtocol.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::IPv4Protocol

prefix_ void senf::IPv4Protocol::connect(INet4Address const & address)
    const
{
    if (::connect(body().fd(),address.sockaddr_p(), address.sockaddr_len()) < 0)
        throw SystemException(errno);
}

prefix_ void senf::IPv4Protocol::bind(INet4Address const & address)
    const
{
    if (::bind(body().fd(),address.sockaddr_p(), address.sockaddr_len()) < 0)
        throw SystemException(errno);
}


///////////////////////////////////////////////////////////////////////////
// senf::IPv6Protocol

prefix_ void senf::IPv6Protocol::connect(INet6SocketAddress const & address)
    const
{
    if (::connect(body().fd(),address.sockaddr_p(), address.sockaddr_len()) < 0)
        throw SystemException(errno);
}

prefix_ void senf::IPv6Protocol::bind(INet6SocketAddress const & address)
    const
{
    if (::bind(body().fd(),address.sockaddr_p(), address.sockaddr_len()) < 0)
        throw SystemException(errno);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "INetProtocol.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
