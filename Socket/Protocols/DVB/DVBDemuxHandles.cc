// $Id$
//
// Copyright (C) 2007
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

/** \file
    \brief DVBDemuxHandles non-inline non-template implementation */

#include "DVBDemuxHandles.hh"
//#include "DVBDemuxHandles.ih"

// Custom includes
#include <boost/format.hpp>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "Utils/Exception.hh"

//#include "DVBDemuxHandles.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::DVBDemuxHandles

prefix_ void senf::DVBDemuxSectionProtocol::init_client(unsigned short adapter, unsigned short device)
    const
{
    std::string devDemux = str( boost::format(
            "/dev/dvb/adapter%d/demux%d") % adapter % device);
    int fd = open(devDemux.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0)
        throw SystemException(errno);
    body().fd(fd);
}

prefix_ unsigned senf::DVBDemuxSectionProtocol::available()
    const
{
    return 4096;
}

prefix_ std::auto_ptr<senf::SocketProtocol> senf::DVBDemuxSectionProtocol::clone()
    const
{
    return std::auto_ptr<SocketProtocol>(new DVBDemuxSectionProtocol());
}

prefix_ void senf::DVBDemuxSectionProtocol::setSectionFilter(struct dmx_sct_filter_params *filter)
    const
{
    if (::ioctl(body().fd(), DMX_SET_FILTER, filter) < 0)
        throw SystemException(errno);
}

// ----------------------------------------------------------------

prefix_ void senf::DVBDemuxPESProtocol::init_client(unsigned short adapter, unsigned short device)
    const
{
    std::string devDemux = str( boost::format(
            "/dev/dvb/adapter%d/demux%d") % adapter % device);
    int fd = open(devDemux.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0)
        throw SystemException(errno);
    body().fd(fd);
}

prefix_ unsigned senf::DVBDemuxPESProtocol::available()
    const
{
    return 4096; //???
}

prefix_ std::auto_ptr<senf::SocketProtocol> senf::DVBDemuxPESProtocol::clone()
    const
{
    return std::auto_ptr<SocketProtocol>(new DVBDemuxPESProtocol());
}

prefix_ void senf::DVBDemuxPESProtocol::setPESFilter(struct dmx_pes_filter_params *filter)
    const
{
    if (::ioctl(body().fd(), DMX_SET_PES_FILTER, filter) < 0)
        throw SystemException(errno);
}

// ----------------------------------------------------------------

prefix_ void senf::DVBDvrProtocol::init_client(unsigned short adapter, unsigned short device)
    const
{
    std::string devDvr = str( boost::format(
            "/dev/dvb/adapter%d/dvr%d") % adapter % device);
    int fd = open(devDvr.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0)
        throw SystemException(errno);
    body().fd(fd);
}

prefix_ unsigned senf::DVBDvrProtocol::available()
    const
{
    return 188;
}

prefix_ std::auto_ptr<senf::SocketProtocol> senf::DVBDvrProtocol::clone()
    const
{
    return std::auto_ptr<SocketProtocol>(new DVBDvrProtocol());
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "DVBDemuxHandles.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
