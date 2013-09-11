// $Id$
//
// Copyright (C) 2007
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
//   David Wagner <dw6@berlios.de>
/** \file
    \brief UNSocketProtocol non-inline non-template implementation */

#include "UNSocketProtocol.hh"
//#include "UNSocketProtocol.ih"

// Custom includes
#include <fstream>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/sockios.h> // for SIOCINQ / SIOCOUTQ
#include <senf/Utils/Logger/SenfLog.hh>
#include <senf/Utils/Exception.hh>
#include <senf/Socket/ClientSocketHandle.hh>
#include "UNAddressing.hh"

//#include "UNSocketProtocol.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
prefix_ unsigned senf::UNSocketProtocol::available()
    const
{
    int n;
    if (::ioctl(fd(),SIOCINQ,&n) < 0)
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
//#include "UNSocketProtocol.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
