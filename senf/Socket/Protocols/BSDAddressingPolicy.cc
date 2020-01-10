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
        SENF_THROW_SYSTEM_EXCEPTION("could not get sockname");
}

prefix_ void senf::BSDAddressingPolicyMixinBase::do_peer(FileHandle const & handle,
                                                                struct sockaddr * addr,
                                                                socklen_t * len)
{
    if (::getpeername(handle.fd(),addr,len) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("could not get peername");
}

prefix_ void senf::BSDAddressingPolicyMixinBase::do_bind(FileHandle const & handle,
                                                                struct sockaddr const * addr,
                                                                socklen_t len)
{
    if (::bind(handle.fd(),addr,len) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("could not bind");
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
                    SENF_THROW_SYSTEM_EXCEPTION("::getsockopt(SO_ERROR)");
                if (err != 0)
                    throw SystemException(err SENF_EXC_DEBUGINFO);
                return;
            }
            case EINTR:
                break;
            default:
                SENF_THROW_SYSTEM_EXCEPTION("could not ::connect");
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
