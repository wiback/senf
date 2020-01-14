//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
        SENF_THROW_SYSTEM_EXCEPTION("could not get sockopt TCP_NODELAY");
    return value;
}

prefix_ void senf::TCPSocketProtocol::nodelay(bool value)
    const
{
    int ivalue (value);
    if (::setsockopt(fd(),SOL_TCP,TCP_NODELAY,&ivalue,sizeof(ivalue)) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("could not set sockopt TCP_NODELAY");
}

prefix_ unsigned senf::TCPSocketProtocol::siocinq()
    const
{
    int n;
    if (::ioctl(fd(),SIOCINQ,&n) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("::ioctl(SIOCINQ)");
    return n;
}

prefix_ unsigned senf::TCPSocketProtocol::siocoutq()
    const
{
    int n;
    if (::ioctl(fd(),SIOCOUTQ,&n) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("::ioctl(SIOCOUTQ)");
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
