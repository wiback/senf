// $Id$
//
// Copyright (C) 2010
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fit.fraunhofer.de/license.html
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
//   Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>

/** \file
    \brief SysInfo non-inline non-template implementation */

#include "SysInfo.hh"

// Custom includes
#include "ScopedDirectory.hh"
#include "ParsedCommand.hh"
#include "Sysdir.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::console::SysInfo::Proxy::Proxy(std::string const & name, std::string const & entry, int pos)
{
    SysInfo::instance().setEntry( name, entry, pos);
}

prefix_ senf::console::SysInfo::SysInfo()
{
    sysdir().add("info", factory::Command(&SysInfo::dump, this));
}

prefix_ void senf::console::SysInfo::setEntry(std::string const & name, std::string const & entry, int pos)
{
    for (Entries::iterator d (entries_.begin()); d != entries_.end(); ++d) {
        if (d->first.second == name) {
            entries_.erase(d);
            break;
        }
    }
    entries_.insert( std::make_pair( std::make_pair(
            pos < 0 ? std::numeric_limits<int>::max() : pos, name), entry));
}

prefix_ void senf::console::SysInfo::dump(std::ostream & os)
    const
{
    for(auto const & entry: entries_) {
        os << entry.second << std::endl;
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
