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
    \brief SysInfo non-inline non-template implementation */

#include "SysInfo.hh"

// Custom includes
#include <boost/foreach.hpp>
#include "ScopedDirectory.hh"
#include "Sysdir.hh"
#include "ParsedCommand.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::console::SysInfo::Proxy::Proxy(std::string const & descr, unsigned pos)
{
    SysInfo::instance().addEntry( descr, pos);
}

prefix_ senf::console::SysInfo::SysInfo()
{
    sysdir().add("info", factory::Command(&SysInfo::dump, this));
}

prefix_ void senf::console::SysInfo::addEntry(std::string const & descr, unsigned pos)
{
    if (pos < 0)
        descr_.push_back( descr);
    else
        descr_.insert( boost::next(descr_.begin(), pos > descr_.size() ? descr_.size() : pos), descr);

}

prefix_ void senf::console::SysInfo::dump(std::ostream & os)
    const
{
    BOOST_FOREACH( std::string const & d, descr_) {
        os << d << std::endl;
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
