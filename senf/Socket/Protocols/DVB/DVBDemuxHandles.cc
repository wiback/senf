//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
