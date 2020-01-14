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
    \brief Beeper non-inline non-template implementation */

#include "Beeper.hh"
//#include "Beeper.ih"

// Custom includes
#include <unistd.h>
#include <fcntl.h>
#include <boost/bind.hpp>
#include "Exception.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::Beeper

prefix_ senf::Beeper::Beeper(std::string const & device)
    : timer_( "senf::Beeper::timer", boost::bind(&Beeper::timeout, this), ClockService::clock_type(0), false)
{
    if ((fd_ = ::open(device.c_str(), O_WRONLY)) == -1) {
        SENF_THROW_SYSTEM_EXCEPTION("Could not open device for Beeper.");
    }
}

prefix_ senf::Beeper::~Beeper()
{
    stop_beep();
    ::close(fd_);
}

prefix_ void senf::Beeper::beep_sync(float freq, unsigned length, unsigned count, unsigned delay)
{
    for (unsigned i = 0; i < count; ++i) {
        if (! start_beep( freq)) return;
        ::usleep( 1000 * length);
        stop_beep();
        if ( i+1 < count)
           ::usleep( 1000 * delay);
    }
}

prefix_ void senf::Beeper::beep_async(float freq, unsigned length, unsigned count, unsigned delay)
{
    if (! start_beep( freq)) return;
    timer_.timeout( senf::ClockService::now() + senf::ClockService::milliseconds(length));
    params_.action = false;  // stop beep
    if (count > 1) {
        params_.freq = freq;
        params_.length = length;
        params_.count = count;
        params_.delay = delay;
    }
}

prefix_ void senf::Beeper::timeout()
{
    if (params_.action) {
        if (! start_beep( params_.freq)) return;
        timer_.timeout( senf::ClockService::now() + senf::ClockService::milliseconds(params_.length));
        params_.action = false;
    } else {
        stop_beep();
        if (--params_.count > 0) {
            timer_.timeout( senf::ClockService::now() + senf::ClockService::milliseconds(params_.delay));
            params_.action = true;
        }
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
