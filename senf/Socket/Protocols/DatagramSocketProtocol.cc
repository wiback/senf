// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fokus.fraunhofer.de.de/license.html
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
//   Stefan Bund <senf@g0dil.de>

/** \file
    \brief DatagramSocketProtocol non-inline non-template implementation */

#include "DatagramSocketProtocol.hh"
//#include "DatagramSocketProtocol.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

//#include "DatagramSocketProtocol.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::ClockService::clock_type senf::DatagramSocketProtocol::timestamp()
    const
{
    struct timeval tv;
    if (::ioctl(fd(), SIOCGSTAMP, &tv) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("could not get timestamp in DatagramSocketProtocol");
    return ClockService::from_timeval(tv);
}

prefix_ void senf::DatagramSocketProtocol::timestamp(struct timespec *spec)
    const
{
    if (::ioctl(fd(), SIOCGSTAMPNS, spec) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("could not get timestamp in DatagramSocketProtocol");
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "DatagramSocketProtocol.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
