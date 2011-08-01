// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Stefan Bund <g0dil@berlios.de>

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
#include <senf/Utils/Exception.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::FileBody

prefix_ void senf::FileBody::close()
{
    if (!valid())
        throw SystemException(EBADF SENF_EXC_DEBUGINFO);
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
        SENF_THROW_SYSTEM_EXCEPTION("could not ::close FileBody fd");
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
    if (flags < 0) SENF_THROW_SYSTEM_EXCEPTION("::fcntl(F_GETFL)");
    return ! (flags & O_NONBLOCK);
}

prefix_ void senf::FileBody::blocking(bool status)
{
    int flags = ::fcntl(fd(),F_GETFL);
    if (flags < 0) SENF_THROW_SYSTEM_EXCEPTION("::fcntl(F_GETFL)");
    if (status) flags &= ~O_NONBLOCK;
    else        flags |= O_NONBLOCK;
    if (::fcntl(fd(), F_SETFL, flags) < 0) SENF_THROW_SYSTEM_EXCEPTION("::fcntl(F_SETFL)");
}

/* We don't take POLLIN/POLLOUT as argument to avoid having to include
   sys/poll.h in the .cci file (and therefore indirectly into the .hh
   and then every file which uses FileHandle) */
prefix_ bool senf::FileBody::pollCheck(int fd, bool incoming, int timeout, bool oob)
    const
{
    struct ::pollfd pfd;
    ::memset(&pfd,0,sizeof(pfd));
    pfd.fd = fd;
    pfd.events = incoming?(oob?POLLPRI:POLLIN):POLLOUT;
    int rv = -1;
    do {
        rv = ::poll(&pfd,1,timeout);
        if (rv<0)
            switch (errno) {
            case EINTR:
                break;
            default:
                SENF_THROW_SYSTEM_EXCEPTION("could not check ::poll");
            }
    } while (rv<0);
    return rv>0;
}

prefix_ senf::FileBody::~FileBody()
{}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
