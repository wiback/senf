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
    \brief SyslogTarget non-inline non-template implementation */

#include "SyslogTarget.hh"
//#include "SyslogTarget.ih"

// Custom includes

//#include "SyslogTarget.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

int const senf::log::SyslogTarget::LEVELMAP_[8] = {
    0, LOG_DEBUG, LOG_INFO, LOG_NOTICE, LOG_WARNING, LOG_CRIT, LOG_EMERG, 0 };

prefix_ void senf::log::SyslogTarget::v_write(time_type timestamp, std::string const & stream,
                                              std::string const & area, unsigned level,
                                              std::string const & message)
{
    if (area != "senf::log::DefaultArea")
        syslog(facility_ | LEVELMAP_[level], "[%s] %s", area.c_str(), message.c_str());
    else
        syslog(facility_ | LEVELMAP_[level], "%s", message.c_str());
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "SyslogTarget.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
