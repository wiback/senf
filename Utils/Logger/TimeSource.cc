// $Id$
//
// Copyright (C) 2008 
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
    \brief TimeSource non-inline non-template implementation */

#include "TimeSource.hh"
#include "TimeSource.ih"

// Custom includes
#include <time.h>
#include "../Exception.hh"

//#include "TimeSource.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::log::TimeSource

prefix_ senf::log::TimeSource::~TimeSource()
{}

///////////////////////////////////////////////////////////////////////////
// senf::log::SystemTimeSource

prefix_ senf::log::time_type senf::log::SystemTimeSource::operator()()
    const
{
    struct ::timespec tm;
    if (::clock_gettime(CLOCK_MONOTONIC, &tm) < 0)
        SENF_THROW_SYSTEM_EXCEPTION("::timer_gettime()");
    return static_cast<time_type>(tm.tv_sec)*1000000000ll+tm.tv_nsec;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "TimeSource.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
