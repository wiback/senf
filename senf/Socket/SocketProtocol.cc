//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief SocketProtocol and ConcreteSocketProtocol non-inline non-template implementation
 */

#include "SocketProtocol.hh"
//#include "SocketProtocol.ih"

// Custom includes
#include <sys/socket.h>

//#include "SocketProtocol.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ void senf::SocketProtocol::close()
{
    if (::close(body().fd()) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("::close(socket_fd)");
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
