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
    \brief DVBDemuxHandles non-inline non-template implementation */

#include "DVBDemuxHandles.hh"
//#include "DVBDemuxHandles.ih"

// Custom includes
#include <boost/format.hpp>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "../../../Utils/Exception.hh"

//#include "DVBDemuxHandles.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::DVBDemuxHandles

prefix_ void senf::DVBDemuxSectionSocketProtocol::init_client(unsigned short adapter, unsigned short device)
    const
{
    std::string devDemux = str( boost::format(
            "/dev/dvb/adapter%d/demux%d") % adapter % device);
    int f = open(devDemux.c_str(), O_RDONLY | O_NONBLOCK);
    if (f < 0){
        SENF_THROW_SYSTEM_EXCEPTION("Could not open demux device of DVB adapter ") << devDemux << ".";
    }
    fd(f);
}

prefix_ unsigned senf::DVBDemuxSectionSocketProtocol::available()
    const
{
    return 4096;
}

prefix_ void senf::DVBDemuxSectionSocketProtocol::setSectionFilter(unsigned short int pid, unsigned int timeout, unsigned int flags, unsigned char filter, unsigned char mask, unsigned char mode)
    const
{
    struct dmx_sct_filter_params sec_filter;
    ::memset(&sec_filter, 0, sizeof(struct dmx_sct_filter_params));
    
       sec_filter.pid = pid;
       sec_filter.filter.filter[0] = filter;
       sec_filter.filter.mask[0] = mask;
       sec_filter.filter.mode[0] = mode;
       sec_filter.flags = flags;
       
       if (::ioctl(fd(), DMX_SET_FILTER, filter) < 0)
           SENF_THROW_SYSTEM_EXCEPTION("Could not set section filter of DVB adapter.");
}

// ----------------------------------------------------------------

prefix_ void senf::DVBDemuxPESSocketProtocol::init_client(unsigned short adapter, unsigned short device)
    const
{
    std::string devDemux = str( boost::format(
            "/dev/dvb/adapter%d/demux%d") % adapter % device);
    int f = open(devDemux.c_str(), O_RDONLY | O_NONBLOCK);
    if (f < 0)
        SENF_THROW_SYSTEM_EXCEPTION("Could not open demux device of DVB adapter ") << devDemux << ".";
    fd(f);
}

prefix_ unsigned senf::DVBDemuxPESSocketProtocol::available()
    const
{
    return 4096; //???
}

prefix_ void senf::DVBDemuxPESSocketProtocol::setPESFilter(struct dmx_pes_filter_params *filter)
    const
{
    if (::ioctl(fd(), DMX_SET_PES_FILTER, filter) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("Could not set PES filter of DVB adapter.");
}

// ----------------------------------------------------------------

prefix_ void senf::DVBDvrSocketProtocol::init_client(unsigned short adapter, unsigned short device)
    const
{
    std::string devDvr = str( boost::format(
            "/dev/dvb/adapter%d/dvr%d") % adapter % device);
    int f = open(devDvr.c_str(), O_RDONLY | O_NONBLOCK);
    if (f < 0)
        SENF_THROW_SYSTEM_EXCEPTION("Could not open dvr device of DVB adapter ") << devDvr << ".";
    fd(f);
}

prefix_ unsigned senf::DVBDvrSocketProtocol::available()
    const
{
    return 188;
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
