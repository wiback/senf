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
    \brief DVBDemuxSocketProtocol non-inline non-template implementation */

#include "DVBDemuxSocketProtocol.hh"
//#include "DVBDemuxSocketProtocol.ih"

// Custom includes
#include <sys/socket.h>
#include <iostream>
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
        SENF_THROW_SYSTEM_EXCEPTION("")
              << "Could not set the size of the buffer on DVB adapter. requested size: "
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
