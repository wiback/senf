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
    \brief BSDSocketProtocol non-inline non-template implementation */

#include "BSDSocketProtocol.hh"
//#include "BSDSocketProtocol.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

//#include "BSDSocketProtocol.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ std::pair<bool,unsigned> senf::BSDSocketProtocol::linger()
    const
{
    struct linger ling;
    socklen_t len = sizeof(ling);
    ::memset(&ling, 0, sizeof(ling));
    if (::getsockopt(fd(),SOL_SOCKET,SO_LINGER,&ling,&len) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
    return std::make_pair(ling.l_onoff, ling.l_linger);
}

prefix_ void senf::BSDSocketProtocol::linger(bool enable, unsigned timeout)
    const
{
    struct linger ling;
    ling.l_onoff = enable;
    ling.l_linger = timeout;
    if (::setsockopt(fd(),SOL_SOCKET,SO_LINGER,&ling,sizeof(ling)) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
}

prefix_ boost::uint8_t senf::BSDSocketProtocol::priority()
    const
{
    int value;
    socklen_t len (sizeof(value));
    if (::getsockopt(fd(),SOL_SOCKET,SO_PRIORITY,&value,&len) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
    return value;
}

prefix_ void senf::BSDSocketProtocol::priority(boost::uint8_t value)
    const
{
    int ivalue (value);
    if (::setsockopt(fd(),SOL_SOCKET,SO_PRIORITY,&ivalue,sizeof(ivalue)) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
}

prefix_ int senf::BSDSocketProtocol::error()
    const
{
    int err;
    socklen_t len (sizeof(err));
    if (::getsockopt(fd(),SOL_SOCKET,SO_ERROR,&err,&len) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
    return err;
}

prefix_ unsigned senf::BSDSocketProtocol::rcvbuf()
    const
{
    unsigned size;
    socklen_t len (sizeof(size));
    if (::getsockopt(fd(),SOL_SOCKET,SO_RCVBUF,&size,&len) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
    // Linux doubles the bufer size on setting the RCVBUF to cater for internal
    // headers. We fix this up here .. (see lkml FAQ)
    return size/2;
}

prefix_ void senf::BSDSocketProtocol::rcvbuf(unsigned size)
    const
{
    if (::setsockopt(fd(),SOL_SOCKET,SO_RCVBUF,&size,sizeof(size)) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
}

prefix_ unsigned senf::BSDSocketProtocol::sndbuf()
    const
{
    unsigned size;
    socklen_t len (sizeof(size));
    if (::getsockopt(fd(),SOL_SOCKET,SO_SNDBUF,&size,&len) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
    // Linux doubles the bufer size on setting the SNDBUF to cater for internal
    // headers. We fix this up here .. (see lkml FAQ)
    return size/2;
}

prefix_ void senf::BSDSocketProtocol::sndbuf(unsigned size)
    const
{
    if (::setsockopt(fd(),SOL_SOCKET,SO_SNDBUF,&size,sizeof(size)) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ bool senf::AddressableBSDSocketProtocol::reuseaddr()
    const
{
    int value;
    socklen_t len (sizeof(value));
    if (::getsockopt(fd(),SOL_SOCKET,SO_REUSEADDR,&value,&len) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
    return value;
}

prefix_ void senf::AddressableBSDSocketProtocol::reuseaddr(bool value)
    const
{
    int ivalue (value);
    if (::setsockopt(fd(),SOL_SOCKET,SO_REUSEADDR,&ivalue,sizeof(ivalue)) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "BSDSocketProtocol.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
