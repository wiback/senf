// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
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
    \brief BSDSocketProtocol non-inline non-template implementation */

#include "BSDSocketProtocol.hh"
//#include "BSDSocketProtocol.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include "../../Socket/SocketHandle.hh"

//#include "BSDSocketProtocol.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ std::pair<bool,unsigned> senf::BSDSocketProtocol::linger()
    const
{
    struct linger ling;
    socklen_t len = sizeof(ling);
    ::memset(&ling,sizeof(ling),0);
    if (::getsockopt(fd(),SOL_SOCKET,SO_LINGER,&ling,&len) < 0)
        throw SystemException();
    return std::make_pair(ling.l_onoff, ling.l_linger);
}

prefix_ void senf::BSDSocketProtocol::linger(bool enable, unsigned timeout)
    const
{
    struct linger ling;
    ling.l_onoff = enable;
    ling.l_linger = timeout;
    if (::setsockopt(fd(),SOL_SOCKET,SO_LINGER,&ling,sizeof(ling)) < 0)
        throw SystemException();
}

prefix_ boost::uint8_t senf::BSDSocketProtocol::priority()
    const
{
    int value;
    socklen_t len (sizeof(value));
    if (::getsockopt(fd(),SOL_SOCKET,SO_PRIORITY,&value,&len) < 0)
        throw SystemException();
    return value;
}

prefix_ void senf::BSDSocketProtocol::priority(boost::uint8_t value)
    const
{
    int ivalue (value);
    if (::setsockopt(fd(),SOL_SOCKET,SO_PRIORITY,&ivalue,sizeof(ivalue)) < 0)
        throw SystemException();
}

prefix_ unsigned senf::BSDSocketProtocol::rcvbuf()
    const
{
    unsigned size;
    socklen_t len (sizeof(size));
    if (::getsockopt(fd(),SOL_SOCKET,SO_RCVBUF,&size,&len) < 0)
        throw SystemException();
    // Linux doubles the bufer size on setting the RCVBUF to cater for internal
    // headers. We fix this up here .. (see lkml FAQ)
    return size/2;
}

prefix_ void senf::BSDSocketProtocol::rcvbuf(unsigned size)
    const
{
    if (::setsockopt(fd(),SOL_SOCKET,SO_RCVBUF,&size,sizeof(size)) < 0)
        throw SystemException();
}

prefix_ unsigned senf::BSDSocketProtocol::sndbuf()
    const
{
    unsigned size;
    socklen_t len (sizeof(size));
    if (::getsockopt(fd(),SOL_SOCKET,SO_SNDBUF,&size,&len) < 0)
        throw SystemException();
    // Linux doubles the bufer size on setting the SNDBUF to cater for internal
    // headers. We fix this up here .. (see lkml FAQ)
    return size/2;
}

prefix_ void senf::BSDSocketProtocol::sndbuf(unsigned size)
    const
{
    if (::setsockopt(fd(),SOL_SOCKET,SO_SNDBUF,&size,sizeof(size)) < 0)
        throw SystemException();
}

///////////////////////////////////////////////////////////////////////////

prefix_ bool senf::AddressableBSDSocketProtocol::reuseaddr()
    const
{
    int value;
    socklen_t len (sizeof(value));
    if (::getsockopt(fd(),SOL_SOCKET,SO_REUSEADDR,&value,&len) < 0)
        throw SystemException();
    return value;
}

prefix_ void senf::AddressableBSDSocketProtocol::reuseaddr(bool value)
    const
{
    int ivalue (value);
    if (::setsockopt(fd(),SOL_SOCKET,SO_REUSEADDR,&ivalue,sizeof(ivalue)) < 0)
        throw SystemException();
}

/////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "BSDSocketProtocol.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
