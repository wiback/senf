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
    \brief BSDAddressingPolicyMixin non-inline non-template implementation
 */

#include "BSDAddressingPolicy.hh"
//#include "BSDAddressingPolicy.ih"

// Custom includes
#include <sys/socket.h>
#include <sys/types.h>
#include <senf/Utils/Exception.hh>

//#include "BSDAddressingPolicy.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ void senf::BSDAddressingPolicyMixinBase::do_local(FileHandle const & handle,
                                                                 struct sockaddr * addr,
                                                                 socklen_t * len)
{
    if (::getsockname(handle.fd(),addr,len) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
}

prefix_ void senf::BSDAddressingPolicyMixinBase::do_peer(FileHandle const & handle,
                                                                struct sockaddr * addr,
                                                                socklen_t * len)
{
    if (::getpeername(handle.fd(),addr,len) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
}

prefix_ void senf::BSDAddressingPolicyMixinBase::do_bind(FileHandle const & handle,
                                                                struct sockaddr const * addr,
                                                                socklen_t len)
{
    if (::bind(handle.fd(),addr,len) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
}

prefix_ void senf::BSDAddressingPolicyMixinBase::do_connect(FileHandle const & handle,
                                                                   struct sockaddr const * addr,
                                                                   socklen_t len)
{
    while(1) {
        if (::connect(handle.fd(),addr,len) < 0)
            switch (errno) {
            case EINPROGRESS: {
                handle.waitWriteable();
                int err = 0;
                socklen_t len = sizeof(err);
                if (::getsockopt(handle.fd(),SOL_SOCKET,SO_ERROR,&err,&len) < 0)
                    SENF_THROW_SYSTEM_EXCEPTION("");
                if (err != 0)
                    throw SystemException(err SENF_EXC_DEBUGINFO);
                return;
            }
            case EINTR:
                break;
            default:
                SENF_THROW_SYSTEM_EXCEPTION("");
            }
        else
            return;
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "BSDAddressingPolicy.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
