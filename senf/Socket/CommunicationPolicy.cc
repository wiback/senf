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
    \brief CommunicationPolicy non-inline non-template implementation
 */

#include "CommunicationPolicy.hh"
//#include "CommunicationPolicy.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <senf/Utils/Exception.hh>

//#include "CommunicationPolicy.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ void senf::ConnectedCommunicationPolicy::do_listen(FileHandle const & handle,
                                                           unsigned backlog)
{
    ::listen(handle.fd(),backlog);
}

prefix_ int senf::ConnectedCommunicationPolicy::do_accept(FileHandle const & handle,
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
