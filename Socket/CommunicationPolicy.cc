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

#include "CommunicationPolicy.hh"
//#include "CommunicationPolicy.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include "Utils/Exception.hh"
#include "ServerSocketHandle.hh"

//#include "CommunicationPolicy.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ int satcom::lib::ConnectedCommunicationPolicy::do_accept(FileHandle handle,
                                                                 struct sockaddr * addr,
                                                                 unsigned len)
{
    int rv = ::accept(handle.fd(),addr,&len);
    if (rv < 0)
        throw SystemException(errno);
    return rv;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "CommunicationPolicy.mpp"


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
