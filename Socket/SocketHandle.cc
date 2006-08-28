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

#include "SocketHandle.hh"
#include "SocketHandle.ih"

// Custom includes
#include <sys/socket.h>

//#include "SocketHandle.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ void satcom::lib::SocketBody::v_close()
{
    if (::shutdown(fd(),SHUT_RDWR) < 0)
        throw SystemException(errno);
    if (::close(fd()) < 0)
        throw SystemException(errno);
}

prefix_ void satcom::lib::SocketBody::v_terminate()
{
    struct linger ling;
    ling.l_onoff = 0;
    ling.l_linger = 0;

    // We purposely IGNORE any errors: this method is used to try and
    // terminate the connection ignoring any possible problems

    ::setsockopt(fd(),SOL_SOCKET,SO_LINGER,&ling,sizeof(ling));
    ::shutdown(fd(),SHUT_RDWR);
    ::close(fd());
}

prefix_ bool satcom::lib::SocketBody::v_eof()
    const
{
    return protocol().eof();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "SocketHandle.mpp"


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
