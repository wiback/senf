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
