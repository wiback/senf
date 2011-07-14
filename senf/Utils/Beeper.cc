// $Id$
//
// Copyright (C) 2010
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
    : timer_( "senf::Beeper::timer", boost::bind(&Beeper::timeout, this), 0, false)
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
