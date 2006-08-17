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

#include "FileHandle.hh"
//#include "FileHandle.ih"

// Custom includes
#include <sys/poll.h>
#include <string.h>
#include <errno.h>
#include "Utils/Exception.hh"

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    bool pollCheck(int fd, int event)
    {
        struct ::pollfd pfd;
        ::memset(&pfd,0,sizeof(pfd));
        pfd.fd = fd;
        pfd.events = event;
        int rv = ::poll(&pfd,1,0);
        if (rv<0)
            throw satcom::lib::SystemException(errno);
        return rv>0;
    }
}

prefix_ bool satcom::lib::FileBody::readable()
    const
{
    return pollCheck(fd(),POLLIN);
}

prefix_ bool satcom::lib::FileBody::writeable()
    const
{
    return pollCheck(fd(),POLLOUT);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
