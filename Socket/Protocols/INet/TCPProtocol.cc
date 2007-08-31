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
    \brief TCPProtocol non-inline non-template implementation
 */

#include "TCPProtocol.hh"
//#include "TCPProtocol.ih"

// Custom includes
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <linux/sockios.h> // for SIOCINQ / SIOCOUTQ
#include "../../../Socket/SocketHandle.hh"

//#include "TCPProtocol.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ bool senf::TCPProtocol::nodelay()
    const
{
    int value;
    socklen_t len (sizeof(value));
    if (::getsockopt(body().fd(),SOL_TCP,TCP_NODELAY,&value,&len) < 0)
        throw SystemException(errno);
    return value;
}

prefix_ void senf::TCPProtocol::nodelay(bool value)
    const
{
    int ivalue (value);
    if (::setsockopt(body().fd(),SOL_TCP,TCP_NODELAY,&ivalue,sizeof(ivalue)) < 0)
        throw SystemException(errno);
}

prefix_ unsigned senf::TCPProtocol::siocinq()
    const
{
    int n;
    if (::ioctl(body().fd(),SIOCINQ,&n) < 0)
        throw SystemException(errno);
    return n;
}

prefix_ unsigned senf::TCPProtocol::siocoutq()
    const
{
    int n;
    if (::ioctl(body().fd(),SIOCOUTQ,&n) < 0)
        throw SystemException(errno);
    return n;
}

prefix_ unsigned senf::TCPProtocol::available()
    const
{
    return siocinq();
}

prefix_ bool senf::TCPProtocol::eof()
    const
{
    return body().readable() && available()==0;
}


///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "TCPProtocol.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
