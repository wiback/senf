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
    \brief DVBFrontendHandle non-inline non-template implementation */

#include "DVBFrontendHandle.hh"
//#include "DVBFrontendHandle.ih"

// Custom includes
#include <boost/format.hpp>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "../../../Utils/Exception.hh"

//#include "DVBFrontendHandle.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::DVBFrontendHandle

prefix_ void senf::DVBFrontendProtocol::init_client(uint8_t adapter, boost::uint8_t device)
    const
{
    std::string devFrontend = str( boost::format(
            "/dev/dvb/adapter%d/frontend%d") % adapter % device);
    int fd = open(devFrontend.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0)
        throwErrno();
    body().fd(fd);
}

prefix_ unsigned senf::DVBFrontendProtocol::available()
    const
{
    return 0;
}

prefix_ bool senf::DVBFrontendProtocol::eof()
    const
{
    return false;
}

prefix_ std::auto_ptr<senf::SocketProtocol> senf::DVBFrontendProtocol::clone()
    const
{
    return std::auto_ptr<SocketProtocol>(new DVBFrontendProtocol());
}


prefix_ void senf::DVBFrontendProtocol::signalStrength(int16_t *strength)
    const
{
    if (::ioctl(body().fd(), FE_READ_SIGNAL_STRENGTH, strength) < 0)
        throwErrno();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "DVBFrontendHandle.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
