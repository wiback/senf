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
    \brief IOEvent non-inline non-template implementation */

#include "IOEvent.hh"
//#include "IOEvent.ih"

// Custom includes
#include "../Utils/senfassert.hh"

//#include "IOEvent.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::ppi::IOEvent

////////////////////////////////////////
// private members

prefix_ void senf::ppi::IOEvent::v_enable()
{
    if (fd_ != -1)
        event_.enable();
}

prefix_ void senf::ppi::IOEvent::v_disable()
{
    if (fd_ != -1)
        event_.disable();
}

prefix_ void senf::ppi::IOEvent::cb(int event)
{
    if ((event & ~event_.events()) != 0) {
        if (event & Err)
            throw ErrorException();
        else if (event & Hup)
            throw HangupException();
        else
            SENF_ASSERT(false && "Internal failure in senf::ppi::IOEvent::cb(int)");
    } else {
        IOEventInfo info = { event };
        callback(info);
    }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "IOEvent.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
