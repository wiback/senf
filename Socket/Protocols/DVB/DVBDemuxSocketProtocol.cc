// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief DVBDemuxSocketProtocol non-inline non-template implementation */

#include "DVBDemuxSocketProtocol.hh"
//#include "DVBDemuxSocketProtocol.ih"

// Custom includes
#include <sys/socket.h>
#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include "../../../Socket/SocketHandle.hh"

//#include "DVBDemuxSocketProtocol.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ void senf::DVBDemuxSocketProtocol::setBufferSize(unsigned long size)
    const
{
    if (::ioctl(fd(), DMX_SET_BUFFER_SIZE, size) < 0)
        throw SystemException(
                "Could not set the size of the buffer on DVB adapter. requested size: ") << size;
}

prefix_ void senf::DVBDemuxSocketProtocol::startFiltering()
    const
{
    if (::ioctl(fd(), DMX_START) < 0)
        throw SystemException("Could not start filtering operation on DVB adapter");
}

prefix_ void senf::DVBDemuxSocketProtocol::stopFiltering()
    const
{
    if (::ioctl(fd(), DMX_STOP) < 0)
        throw SystemException("Could not stop filtering operation on DVB adapter");
}

prefix_ bool senf::DVBDemuxSocketProtocol::eof()
    const
{
    return false;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "DVBDemuxSocketProtocol.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
