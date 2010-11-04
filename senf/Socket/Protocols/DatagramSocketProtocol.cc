// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
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
        SENF_THROW_SYSTEM_EXCEPTION("");
    return ClockService::from_timeval(tv);
}

prefix_ senf::ClockService::clock_type senf::DatagramSocketProtocol::timestamp_system()
    const
{
    // we use gettimeofday() here, since on some boxes the above ioctl() return bogus values.
    // this may reduce the precision, but we only care about +/- 1ms, for now 
    struct timeval tv;
    ::gettimeofday( &tv, NULL);
    return tv.tv_sec * 1000000000LL + tv.tv_usec * 1000LL;
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
