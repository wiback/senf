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
    \brief CommunicationPolicy non-inline non-template implementation
 */

#include "CommunicationPolicy.hh"
//#include "CommunicationPolicy.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include "../Utils/Exception.hh"
#include "ServerSocketHandle.hh"

//#include "CommunicationPolicy.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ void senf::ConnectedCommunicationPolicy::do_listen(FileHandle handle,
                                                           unsigned backlog)
{
    ::listen(handle.fd(),backlog);
}

prefix_ int senf::ConnectedCommunicationPolicy::do_accept(FileHandle handle,
                                                          struct sockaddr * addr,
                                                          unsigned len)
{
    int rv = -1;
    do {
        rv = ::accept(handle.fd(),addr,&len);
        if (rv < 0)
            switch (errno) {
            case EWOULDBLOCK:
                return -1;
            case EINTR:
                break;
            default:
                SENF_THROW_SYSTEM_EXCEPTION("ConnectedCommunicationPolicy::do_accept failed.");
            }
    } while (rv<0);
    return rv;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "CommunicationPolicy.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
