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
    \brief ConfigFile non-inline non-template implementation */

#include "ConfigFile.hh"
#include "ConfigFile.ih"

// Custom includes

//#include "ConfigFile.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::console::detail::ConfigFileSource

prefix_ void senf::console::detail::ConfigFileSource::v_parse(RestrictedExecutor & executor)
{
     parser_.parseFile(filename_, boost::bind( boost::ref(executor),
                                               boost::ref(std::cerr),
                                               _1 ));
}

///////////////////////////////////////////////////////////////////////////

prefix_ void senf::console::parseFile(std::string const & filename, DirectoryNode & root)
{
    ConfigFile cfg (filename, root);
    cfg.parse();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "ConfigFile.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
