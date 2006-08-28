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

#include "FileHandle.hh"
//#include "FileHandle.ih"

// Custom includes
#include <sys/poll.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "Utils/Exception.hh"

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ bool satcom::lib::FileBody::blocking()
    const
{
    int flags = ::fcntl(fd(),F_GETFL);
    if (flags < 0) throw SystemException(errno);
    return ! (flags & O_NONBLOCK);
}

prefix_ bool satcom::lib::FileBody::blocking(bool status)
    const
{
    int flags = ::fcntl(fd(),F_GETFL);
    if (flags < 0) throw SystemException(errno);
    bool old = flags & O_NONBLOCK;
    if (status) flags &= ~O_NONBLOCK;
    else        flags |= O_NONBLOCK;
    if (::fcntl(fd(), F_SETFL, flags) < 0) throw SystemException(errno);
    return ! old;
}

prefix_ bool satcom::lib::FileBody::pollCheck(int fd, bool incoming, bool block)
    const
{
    struct ::pollfd pfd;
    ::memset(&pfd,0,sizeof(pfd));
    pfd.fd = fd;
    pfd.events = incoming?POLLIN:POLLOUT;
    int rv = ::poll(&pfd,1,block?-1:0);
    if (rv<0)
        throw satcom::lib::SystemException(errno);
    return rv>0;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
