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
    \brief BSDAddressingPolicyMixin non-inline non-template implementation
 */

#include "BSDAddressingPolicy.hh"
//#include "BSDAddressingPolicy.ih"

// Custom includes
#include <sys/socket.h>
#include <sys/types.h>
#include "../../Utils/Exception.hh"

//#include "BSDAddressingPolicy.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ void senf::BSDAddressingPolicyMixinBase::do_local(FileHandle handle,
                                                                 struct sockaddr * addr,
                                                                 socklen_t * len)
{
    if (::getsockname(handle.fd(),addr,len) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
}

prefix_ void senf::BSDAddressingPolicyMixinBase::do_peer(FileHandle handle,
                                                                struct sockaddr * addr,
                                                                socklen_t * len)
{
    if (::getpeername(handle.fd(),addr,len) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
}

prefix_ void senf::BSDAddressingPolicyMixinBase::do_bind(FileHandle handle,
                                                                struct sockaddr const * addr,
                                                                socklen_t len)
{
    if (::bind(handle.fd(),addr,len) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
}

prefix_ void senf::BSDAddressingPolicyMixinBase::do_connect(FileHandle handle,
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

///////////////////////////////cc.e////////////////////////////////////////
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
