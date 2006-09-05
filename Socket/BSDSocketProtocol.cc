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

#include "BSDSocketProtocol.hh"
//#include "BSDSocketProtocol.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include "SocketHandle.hh"

//#include "BSDSocketProtocol.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ std::pair<bool,unsigned> satcom::lib::BSDSocketProtocol::linger()
    const
{
    struct linger ling;
    socklen_t len = sizeof(ling);
    ::memset(&ling,sizeof(ling),0);
    if (::getsockopt(body().fd(),SOL_SOCKET,SO_LINGER,&ling,&len) < 0)
        throw SystemException(errno);
    return std::make_pair(ling.l_onoff, ling.l_linger);
}

prefix_ void satcom::lib::BSDSocketProtocol::linger(bool enable, unsigned timeout)
    const
{
    struct linger ling;
    ling.l_onoff = enable;
    ling.l_linger = timeout;
    if (::setsockopt(body().fd(),SOL_SOCKET,SO_LINGER,&ling,sizeof(ling)) < 0)
        throw SystemException(errno);
}

prefix_ struct timeval satcom::lib::BSDSocketProtocol::timestamp()
    const
{
    // TODO: Check, why this fails with ENOFILE (!!!!) at least when
    // called from a tcp socket. Further investigation necessary ...
    struct timeval tv;
    if (::ioctl(body().fd(), SIOCGSTAMP, &tv) < 0)
        throw SystemException(errno);
    return tv;
}

///////////////////////////////////////////////////////////////////////////

prefix_ bool satcom::lib::AddressableBSDSocketProtocol::reuseaddr()
    const
{
    int value;
    socklen_t len (sizeof(value));
    if (::getsockopt(body().fd(),SOL_SOCKET,SO_REUSEADDR,&value,&len) < 0)
        throw SystemException(errno);
    return value;
}

prefix_ void satcom::lib::AddressableBSDSocketProtocol::reuseaddr(bool value)
    const
{
    int ivalue (value);
    if (::setsockopt(body().fd(),SOL_SOCKET,SO_REUSEADDR,&ivalue,sizeof(ivalue)) < 0)
        throw SystemException(errno);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "BSDSocketProtocol.mpp"


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
