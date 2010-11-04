// $Id$
//
// Copyright (C) 2010
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
