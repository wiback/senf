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
    \brief TCPSocketProtocol non-inline non-template implementation
 */

#include "TCPSocketProtocol.hh"
//#include "TCPSocketProtocol.ih"

// Custom includes
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <linux/sockios.h> // for SIOCINQ / SIOCOUTQ
#include "../../../Socket/SocketHandle.hh"

//#include "TCPSocketProtocol.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ bool senf::TCPSocketProtocol::nodelay()
    const
{
    int value;
    socklen_t len (sizeof(value));
    if (::getsockopt(fd(),SOL_TCP,TCP_NODELAY,&value,&len) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
    return value;
}

prefix_ void senf::TCPSocketProtocol::nodelay(bool value)
    const
{
    int ivalue (value);
    if (::setsockopt(fd(),SOL_TCP,TCP_NODELAY,&ivalue,sizeof(ivalue)) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
}

prefix_ unsigned senf::TCPSocketProtocol::siocinq()
    const
{
    int n;
    if (::ioctl(fd(),SIOCINQ,&n) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
    return n;
}

prefix_ unsigned senf::TCPSocketProtocol::siocoutq()
    const
{
    int n;
    if (::ioctl(fd(),SIOCOUTQ,&n) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
    return n;
}

prefix_ unsigned senf::TCPSocketProtocol::available()
    const
{
    return siocinq();
}

prefix_ bool senf::TCPSocketProtocol::eof()
    const
{
    return fh().readable() && available()==0;
}


///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "TCPSocketProtocol.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
