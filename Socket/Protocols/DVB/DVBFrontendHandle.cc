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

prefix_ void senf::DVBFrontendSocketProtocol::init_client(uint8_t adapter, boost::uint8_t device)
    const
{
    std::string devFrontend = str( boost::format(
            "/dev/dvb/adapter%d/frontend%d") % adapter % device);
    int f = open(devFrontend.c_str(), O_RDONLY | O_NONBLOCK);
    if (f < 0)
        SENF_THROW_SYSTEM_EXCEPTION("") << "Could not open frontend device of DVB adapter " << devFrontend << ".";
    
    fd(f);
}

prefix_ unsigned senf::DVBFrontendSocketProtocol::available()
    const
{
    return 0;
}

prefix_ bool senf::DVBFrontendSocketProtocol::eof()
    const
{
    return false;
}

prefix_ void senf::DVBFrontendSocketProtocol::signalStrength(int16_t *strength)
    const
{
    if (::ioctl(fd(), FE_READ_SIGNAL_STRENGTH, strength) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("Could not get signal strength of DVB adapter.");
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
