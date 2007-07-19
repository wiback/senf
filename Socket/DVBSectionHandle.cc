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
    \brief xxx
 */

#include "DVBSectionHandle.hh"
//#include "DVBSectionHandle.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <stdio.h>
#include <fcntl.h>

#include "Utils/Exception.hh"

//#include "DVBSectionHandle.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::DVBSectionProtocol

prefix_ void senf::DVBSectionProtocol::init_client()
    const
{
    int fd = open("/dev/dvb/adapter0/demux0", O_RDONLY | O_NONBLOCK);
    if (fd < 0)
        throw SystemException(errno);
    body().fd(fd);
}

prefix_ unsigned senf::DVBSectionProtocol::available()
    const
{
    return 4096;
}

prefix_ std::auto_ptr<senf::SocketProtocol> senf::DVBSectionProtocol::clone()
    const
{
    return std::auto_ptr<SocketProtocol>(new DVBSectionProtocol());
}


prefix_ void senf::DVBSectionProtocol::setSectionFilter(
        unsigned short pid,
        unsigned char table_id)
    const
{
    struct dmx_sct_filter_params sec_filter;
    memset(&sec_filter, 0, sizeof (struct dmx_sct_filter_params));
    sec_filter.pid = pid;
    sec_filter.filter.filter[0] = table_id;
    sec_filter.filter.mask[0] = 0xff;
    sec_filter.flags = DMX_IMMEDIATE_START;
    sec_filter.flags |= DMX_CHECK_CRC;
    setSectionFilter( &sec_filter );
}


prefix_ void senf::DVBSectionProtocol::setSectionFilter(struct dmx_sct_filter_params *filter)
    const
{
    if (::ioctl(body().fd(), DMX_SET_FILTER, filter) < 0)
        throw SystemException(errno);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "DVBSectionHandle.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
