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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief IOEvent non-inline non-template implementation */

#include "IOEvent.hh"
//#include "IOEvent.ih"

// Custom includes
#include <senf/Utils/senfassert.hh>

//#include "IOEvent.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::IOEvent

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
        if (event & Hup)
            throw HangupException();
        SENF_ASSERT(false, "Internal failure: IOEvent::cb() called with invalid flag??");
    } else {
        IOEventInfo info = { event };
        callback(info);
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
