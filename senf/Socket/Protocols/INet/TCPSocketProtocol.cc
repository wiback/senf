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
    \brief TCPSocketProtocol non-inline non-template implementation
 */

#include "TCPSocketProtocol.hh"
//#include "TCPSocketProtocol.ih"

// Custom includes
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <linux/sockios.h> // for SIOCINQ / SIOCOUTQ

//#include "TCPSocketProtocol.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ bool senf::TCPSocketProtocol::nodelay()
    const
{
    int value;
    socklen_t len (sizeof(value));
    if (::getsockopt(fd(),SOL_TCP,TCP_NODELAY,&value,&len) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
    return value;
}

prefix_ void senf::TCPSocketProtocol::nodelay(bool value)
    const
{
    int ivalue (value);
    if (::setsockopt(fd(),SOL_TCP,TCP_NODELAY,&ivalue,sizeof(ivalue)) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
}

prefix_ unsigned senf::TCPSocketProtocol::siocinq()
    const
{
    int n;
    if (::ioctl(fd(),SIOCINQ,&n) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
    return n;
}

prefix_ unsigned senf::TCPSocketProtocol::siocoutq()
    const
{
    int n;
    if (::ioctl(fd(),SIOCOUTQ,&n) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("");
    return n;
}

prefix_ void senf::TCPSocketProtocol::shutdown(ShutType type)
    const
{
    if (::shutdown(fd(), type) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("::shutdown()");
}

prefix_ void senf::TCPSocketProtocol::close()
{
    shutdown(ShutRDWR);
    INetSocketProtocol::close();
}

prefix_ unsigned senf::TCPSocketProtocol::available()
    const
{
    return siocinq();
}

prefix_ bool senf::TCPSocketProtocol::eof()
    const
{
    return fh().readable() && available()==0;
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "TCPSocketProtocol.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
