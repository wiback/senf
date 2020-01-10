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
    \brief UNSocketProtocol non-inline non-template implementation */

#include "UNSocketProtocol.hh"

// Custom includes
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/sockios.h> // for SIOCINQ / SIOCOUTQ
#include <senf/Utils/Logger/SenfLog.hh>
#include <senf/Utils/Exception.hh>
#include <senf/Socket/ClientSocketHandle.hh>
#include "UNAddressing.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
prefix_ unsigned senf::UNSocketProtocol::available()
    const
{
    int n;
    if (::ioctl(fd(), SIOCINQ, &n) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("Could not call available() on UNSocket.");
    return n;
}

prefix_ bool senf::UNSocketProtocol::eof()
    const
{
    return false;
}

prefix_ void senf::UNSocketProtocol::close()
{
    check_and_unlink();

    SocketProtocol::close();
}

prefix_ void senf::UNSocketProtocol::terminate()
    const
{
    check_and_unlink();

    SocketProtocol::terminate();
}

prefix_ void senf::UNSocketProtocol::check_and_unlink()
    const
{
    typedef ClientSocketHandle<MakeSocketPolicy<UNAddressingPolicy>::policy> UNSocketHandle;
    try {
        UNSocketAddress una (static_socket_cast<UNSocketHandle>(fh()).local());
        ::unlink(una.path().c_str());
    }
    catch (SystemException & e) {
        SENF_LOG(("UNSocketProtocol::check_and_unlink() failed; [" << e.errorString() << "]."));
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
