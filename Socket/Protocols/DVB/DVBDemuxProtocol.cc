// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>
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
    \brief DVBDemuxProtocol non-inline non-template implementation */

#include "DVBDemuxProtocol.hh"
//#include "DVBDemuxProtocol.ih"

// Custom includes
#include <sys/socket.h>
#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include "../../../Socket/SocketHandle.hh"

//#include "DVBDemuxProtocol.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ void senf::DVBDemuxProtocol::setBufferSize(unsigned long size)
    const
{
    if (::ioctl(body().fd(), DMX_SET_BUFFER_SIZE, size) < 0)
        throwErrno();
}

prefix_ void senf::DVBDemuxProtocol::startFiltering()
    const
{
    if (::ioctl(body().fd(), DMX_START) < 0)
        throwErrno();
}

prefix_ void senf::DVBDemuxProtocol::stopFiltering()
    const
{
    if (::ioctl(body().fd(), DMX_STOP) < 0)
        throwErrno();
}

prefix_ bool senf::DVBDemuxProtocol::eof()
    const
{
    return false;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "DVBDemuxProtocol.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
