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
    \brief FileHandle non-inline non-template implementation
 */

#include "FileHandle.hh"
//#include "FileHandle.ih"

// Custom includes
#include <unistd.h>
#include <sys/poll.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "../Utils/Exception.hh"

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::FileBody

prefix_ void senf::FileBody::close()
{
    if (!valid())
        throwErrno(EBADF);
    v_close();
    fd_ = -1;
}

prefix_ void senf::FileBody::terminate()
{
    if (valid()) {
        v_terminate();
        fd_ = -1;
    }
}

prefix_ void senf::FileBody::destroyClose()
{
    if (valid())
        try {
            close();
        }
        catch (...) {
            terminate();
        }
}

prefix_ void senf::FileBody::v_close()
{
    if (::close(fd_) != 0)
        throwErrno();
}

prefix_ void senf::FileBody::v_terminate()
{
    ::close(fd_);
}

prefix_ bool senf::FileBody::v_eof()
    const
{
    return false;
}

prefix_ bool senf::FileBody::v_valid()
    const
{
    return true;
}

prefix_ bool senf::FileBody::blocking()
    const
{
    int flags = ::fcntl(fd(),F_GETFL);
    if (flags < 0) throwErrno();
    return ! (flags & O_NONBLOCK);
}

prefix_ void senf::FileBody::blocking(bool status)
{
    int flags = ::fcntl(fd(),F_GETFL);
    if (flags < 0) throwErrno();
    if (status) flags &= ~O_NONBLOCK;
    else        flags |= O_NONBLOCK;
    if (::fcntl(fd(), F_SETFL, flags) < 0) throwErrno();
}

/* We don't take POLLIN/POLLOUT as argument to avoid having to include
   sys/poll.h in the .cci file (and therefore indirectly into the .hh
   and then every file which uses FileHandle) */
prefix_ bool senf::FileBody::pollCheck(int fd, bool incoming, bool block)
    const
{
    struct ::pollfd pfd;
    ::memset(&pfd,0,sizeof(pfd));
    pfd.fd = fd;
    pfd.events = incoming?POLLIN:POLLOUT;
    int rv = -1;
    do {
        rv = ::poll(&pfd,1,block?-1:0);
        if (rv<0)
            switch (errno) {
            case EINTR:
                break;
            default:
                throwErrno();
            }
    } while (rv<0);
    return rv>0;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
