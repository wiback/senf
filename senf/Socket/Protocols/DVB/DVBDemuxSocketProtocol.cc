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
    \brief DVBDemuxSocketProtocol non-inline non-template implementation */

#include "DVBDemuxSocketProtocol.hh"
//#include "DVBDemuxSocketProtocol.ih"

// Custom includes
#include <sys/socket.h>
#include <string>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <senf/Socket/SocketHandle.hh>

//#include "DVBDemuxSocketProtocol.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ void senf::DVBDemuxSocketProtocol::setBufferSize(unsigned long size)
    const
{
    if (::ioctl(fd(), DMX_SET_BUFFER_SIZE, size) < 0)
        SENF_THROW_SYSTEM_EXCEPTION(
                "Could not set the size of the buffer on DVB adapter. requested size: ")
                << size << ".";
}

prefix_ void senf::DVBDemuxSocketProtocol::startFiltering()
    const
{
    if (::ioctl(fd(), DMX_START) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("Could not start filtering operation on DVB adapter.");
}

prefix_ void senf::DVBDemuxSocketProtocol::stopFiltering()
    const
{
    if (::ioctl(fd(), DMX_STOP) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("Could not stop filtering operation on DVB adapter.");
}

prefix_ bool senf::DVBDemuxSocketProtocol::eof()
    const
{
    return false;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
