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
    \brief ReadPolicy and WritePolicy non-inline non-template implementation
 */

#include "ReadWritePolicy.hh"
//#include "ReadWritePolicy.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>


//#include "ReadWritePolicy.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ unsigned senf::ReadablePolicy::read(FileHandle handle, char * buffer,
                                                   unsigned size)
{
    int rv = -1;
    do {
        rv = ::read(handle.fd(), buffer, size);
        if (rv < 0)
            switch(errno) {
            case EINTR:
                break;
            case EAGAIN:
                // This means, the socket is non-blocking an no data was available
                rv = 0;
                break;
            default:
                throw SystemException(errno);
            }
    } while (rv<0);
    return rv;
}

prefix_ unsigned senf::ReadablePolicy::do_readfrom(FileHandle handle, char * buffer,
                                                          unsigned size,
                                                          struct ::sockaddr * addr, socklen_t len)
{
    int rv = -1;
    do {
        rv = ::recvfrom(handle.fd(),buffer, size, 0, addr, &len);
        if (rv < 0)
            switch (errno) {
            case EINTR:
                break;
            case EAGAIN:
                rv = 0;
                break;
            default:
                throw SystemException(errno);
            }
    } while (rv<0);
    return rv;
}

prefix_ unsigned senf::WriteablePolicy::do_write(FileHandle handle, char const * buffer,
                                                        unsigned size)
{
    int rv = -1;
    do {
        rv = ::write(handle.fd(), buffer, size);
        if (rv < 0)
            switch (errno) {
            case EINTR:
                break;
            case EAGAIN:
                rv = 0;
                break;
            default:
                throw SystemException(errno);
            }
    } while (rv<0);
    return rv;
}

prefix_ unsigned senf::WriteablePolicy::do_writeto(FileHandle handle,
                                                          char const * buffer, unsigned size,
                                                          struct sockaddr * addr, socklen_t len)
{
    int rv = -1;
    do {
        rv = ::sendto(handle.fd(), buffer, size, 0, addr, len);
        if (rv < 0)
            switch (errno) {
            case EINTR:
                break;
            case EAGAIN:
                rv = 0;
                break;
            default:
                throw SystemException(errno);
            }
    } while (rv<0);
    return rv;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "ReadWritePolicy.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
