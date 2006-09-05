// $Id$
//
// Copyright (C) 2006 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

// Definition of non-inline non-template functions

#include "GenericAddressingPolicy.hh"
//#include "GenericAddressingPolicy.ih"

// Custom includes
#include <sys/socket.h>
#include <sys/types.h>
#include "Utils/Exception.hh"

//#include "GenericAddressingPolicy.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ void satcom::lib::GenericAddressingPolicy_Base::do_local(FileHandle handle,
                                                                 struct sockaddr * addr,
                                                                 unsigned len)
{
    if (::getsockname(handle.fd(),addr,&len) < 0)
        throw SystemException(errno);
}

prefix_ void satcom::lib::GenericAddressingPolicy_Base::do_peer(FileHandle handle,
                                                                struct sockaddr * addr,
                                                                unsigned len)
{
    if (::getpeername(handle.fd(),addr,&len) < 0)
        throw SystemException(errno);
}

prefix_ void satcom::lib::GenericAddressingPolicy_Base::do_bind(FileHandle handle,
                                                                struct sockaddr const * addr,
                                                                unsigned len)
{
    if (::bind(handle.fd(),addr,len) < 0)
        throw SystemException(errno);
}

prefix_ void satcom::lib::GenericAddressingPolicy_Base::do_connect(FileHandle handle,
                                                                   struct sockaddr const * addr,
                                                                   unsigned len)
{
    // FIXME: a non-blocking socket will return with EINPROGRESS
    // This necessitates reading the SO_ERROR value and poll-ing for
    // completion !! (see man connect)
    if (::connect(handle.fd(),addr,len) < 0)
        throw SystemException(errno);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "GenericAddressingPolicy.mpp"


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
