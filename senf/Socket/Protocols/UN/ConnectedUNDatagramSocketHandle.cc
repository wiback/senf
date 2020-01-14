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
    \brief ConnectedUNDatagramSocketHandle non-inline non-template implementation */

#include "ConnectedUNDatagramSocketHandle.hh"

// Custom includes
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <senf/Utils/Exception.hh>

//#include "UNDatagramSocketHandle.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ void senf::ConnectedUNDatagramSocketProtocol::init_client() const
{
    int sock = ::socket(PF_UNIX, SOCK_DGRAM, 0);
    if (sock < 0)
        SENF_THROW_SYSTEM_EXCEPTION("Could not create socket(PF_UNIX,SOCK_DGRAM,0).");
    fd(sock);
}

prefix_ void senf::ConnectedUNDatagramSocketProtocol::init_client(UNSocketAddress const & address) const
{
    init_client();
    clientHandle().connect(address);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "UNDatagramSocketHandle.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
