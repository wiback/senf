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
    \brief SocketProtocol and ConcreteSocketProtocol non-inline non-template implementation
 */

#include <sys/socket.h>
#include "SocketProtocol.hh"
//#include "SocketProtocol.ih"

// Custom includes

//#include "SocketProtocol.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ void senf::SocketProtocol::close()
    const
{
    if (::shutdown(body().fd(),SHUT_RDWR) < 0)
        throw SystemException();
    if (::close(body().fd()) < 0)
        throw SystemException();
}

prefix_ void senf::SocketProtocol::terminate()
    const
{
    struct linger ling;
    ling.l_onoff = 0;
    ling.l_linger = 0;

    // We purposely IGNORE any errors: this method is used to try and
    // terminate the connection ignoring any possible problems

    ::setsockopt(body().fd(),SOL_SOCKET,SO_LINGER,&ling,sizeof(ling));
    ::shutdown(body().fd(),SHUT_RDWR);
    ::close(body().fd());
}

prefix_ void senf::SocketProtocol::state(SocketStateMap & map, unsigned lod)
    const
{}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "SocketProtocol.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
