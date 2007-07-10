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
    \brief BufferingPolicy non-inline non-template implementation
 */

#include "BufferingPolicy.hh"
//#include "BufferingPolicy.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include "Utils/Exception.hh"

//#include "BufferingPolicy.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ unsigned senf::SocketBufferingPolicy::rcvbuf(FileHandle handle)
{
    unsigned size;
    socklen_t len (sizeof(size));
    if (::getsockopt(handle.fd(),SOL_SOCKET,SO_RCVBUF,&size,&len) < 0)
        throw SystemException(errno);
    // Linux doubles the bufer size on setting the RCVBUF to cater for internal
    // headers. We fix this up here .. (see lkml FAQ)
    return size/2;
}

prefix_ void senf::SocketBufferingPolicy::rcvbuf(FileHandle handle, unsigned size)
{
    if (::setsockopt(handle.fd(),SOL_SOCKET,SO_RCVBUF,&size,sizeof(size)) < 0)
        throw SystemException(errno);
}

prefix_ unsigned senf::SocketBufferingPolicy::sndbuf(FileHandle handle)
{
    unsigned size;
    socklen_t len (sizeof(size));
    if (::getsockopt(handle.fd(),SOL_SOCKET,SO_SNDBUF,&size,&len) < 0)
        throw SystemException(errno);
    // Linux doubles the bufer size on setting the SNDBUF to cater for internal
    // headers. We fix this up here .. (see lkml FAQ)
    return size/2;

}

prefix_ void senf::SocketBufferingPolicy::sndbuf(FileHandle handle, unsigned size)
{
    if (::setsockopt(handle.fd(),SOL_SOCKET,SO_SNDBUF,&size,sizeof(size)) < 0)
        throw SystemException(errno);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "BufferingPolicy.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
