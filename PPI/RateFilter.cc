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
    \brief ppitest non-inline non-template implementation */

#include "RateFilter.hh"

#define prefix_
// ////////////////////////////////////////////////////////////////////////
// RateFilter


senf::ppi::module::RateFilter::RateFilter(senf::ClockService::clock_type interval)
    : timer(interval) 
{
    route(input,timer);
    route(timer,output);
    registerEvent(timer, &RateFilter::timeout);
}

void senf::ppi::module::RateFilter::timeout()
{
    output(input());
}


/*  this should be what should happen. but _this_ most likely won't work
void senf::ppi::module::RateFilter::changeInterval(senf::ClockService::clock_type interval)
{
    //timer = ppi::IntervalTimer(interval);
}
*/
