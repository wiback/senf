// $Id$
//
// Copyright (C) 2007 
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
    \brief UNProtocol non-inline non-template implementation */

#include "UNProtocol.hh"
//#include "UNProtocol.ih"

// Custom includes
#include <sys/socket.h>
#include "../../../Utils/Exception.hh"

//#include "UNProtocol.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////
prefix_ void senf::UNProtocol::connect(UNSocketAddress const & address) 
    const 
{
    if(::connect(body().fd(),  address.sockaddr_p(), sizeof(address.sockaddr())) < 0)
        throw SystemException(errno);
}

prefix_ void senf::UNProtocol::bind(UNSocketAddress const & address) 
    const 
{
    if(::bind(body().fd(), address.sockaddr_p(), sizeof(address.sockaddr())) < 0)
        throw SystemException(errno);
}





///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "UNProtocol.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
