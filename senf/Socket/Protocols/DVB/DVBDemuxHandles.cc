// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief DVBDemuxHandles non-inline non-template implementation */

#include "DVBDemuxHandles.hh"
//#include "DVBDemuxHandles.ih"

// Custom includes
#include <boost/format.hpp>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <senf/Utils/Exception.hh>


//#include "DVBDemuxHandles.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::DVBDemuxHandles

prefix_ void senf::DVBDemuxSectionSocketProtocol::init_client(unsigned short adapter, unsigned short device)
    const
{
    std::string devDemux = str( boost::format("/dev/dvb/adapter%d/demux%d") % adapter % device);
    int f = open(devDemux.c_str(), O_RDONLY | O_NONBLOCK);
    if (f < 0) {
        SENF_THROW_SYSTEM_EXCEPTION("Could not open demux device of DVB adapter ") << devDemux << ".";
    }
    fd(f);
}

prefix_ unsigned senf::DVBDemuxSectionSocketProtocol::available()
    const
{
    return 4096;
}

prefix_ void senf::DVBDemuxSectionSocketProtocol::setSectionFilter(unsigned short int pid, unsigned char filter, unsigned int flags, unsigned char mask, unsigned char mode, unsigned int timeout)
    const
{
    struct dmx_sct_filter_params sec_filter;
    ::memset(&sec_filter, 0, sizeof(struct dmx_sct_filter_params));
    sec_filter.pid = pid;
    sec_filter.filter.filter[0] = filter;
    sec_filter.filter.mask[0] = mask;
    sec_filter.filter.mode[0] = mode;
    sec_filter.flags = flags;

    if (::ioctl(fd(), DMX_SET_FILTER, &sec_filter) < 0)
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

prefix_ void senf::DVBDemuxPESSocketProtocol::setPESFilter(unsigned short int pid, dmx_input_t input, dmx_output_t output, dmx_pes_type_t pesType, unsigned int flags)
    const
{

    struct dmx_pes_filter_params pes_filter;
        ::memset(&pes_filter, 0, sizeof(struct dmx_pes_filter_params));
        pes_filter.pid = pid;
        pes_filter.input = input;
        pes_filter.output = output;
        pes_filter.pes_type = pesType;
        pes_filter.flags = flags;

    if (::ioctl(fd(), DMX_SET_PES_FILTER, &pes_filter) < 0)
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
