// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     David Wagner <dw6@berlios.de>
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
    \brief UNSocketProtocol non-inline non-template implementation */

#include "UNSocketProtocol.hh"
//#include "UNSocketProtocol.ih"

// Custom includes
#include <fstream>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/sockios.h> // for SIOCINQ / SIOCOUTQ
#include <senf/Utils/Logger.hh>
#include "../../../Utils/Exception.hh"

//#include "UNSocketProtocol.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////
prefix_ unsigned senf::UNSocketProtocol::available()
    const
{
    int n;
    if (::ioctl(fd(),SIOCINQ,&n) < 0)
        throw SystemException("Could not call available() on UNSocket.");
    return n;
}

prefix_ bool senf::UNSocketProtocol::eof()
    const
{
    return false;
}

prefix_ void senf::UNSocketProtocol::close() 
    const
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
    
///////////////////////////////cc.e////////////////////////////////////////
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
