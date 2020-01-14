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
