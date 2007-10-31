// $Id$
//
// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer NETwork research (NET)
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
    \brief FileTarget non-inline non-template implementation */

#include "FileTarget.hh"
//#include "FileTarget.ih"

// Custom includes

//#include "FileTarget.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::log::FileTarget::FileTarget(std::string file)
    : ofstream_t(file.c_str(), std::ofstream::app), IOStreamTarget(ofstream_t::member), file_(file)
{}

prefix_ void senf::log::FileTarget::reopen()
{
    ofstream_t::member.close();
    ofstream_t::member.open(file_.c_str(), std::ofstream::app);
}

prefix_ void senf::log::FileTarget::reopen(std::string file)
{
    file_ = file;
    reopen();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "FileTarget.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
