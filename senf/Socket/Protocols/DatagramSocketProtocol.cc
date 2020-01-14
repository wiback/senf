//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief DatagramSocketProtocol non-inline non-template implementation */

#include "DatagramSocketProtocol.hh"
//#include "DatagramSocketProtocol.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <senf/Scheduler/Scheduler.hh>

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
