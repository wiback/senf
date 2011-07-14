// $Id$
//
// Copyright (C) 2008
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
    \brief ConfigFile non-inline non-template implementation */

#include "ConfigFile.hh"
#include "ConfigFile.ih"

// Custom includes
#include <boost/bind.hpp>

//#include "ConfigFile.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::console::detail::ConfigFileSource
#ifndef DOXYGEN

prefix_ void senf::console::detail::ConfigFileSource::v_parse(RestrictedExecutor & executor)
{
    try {
        parser_.parseFile(filename_, boost::bind( boost::ref(executor),
                                                  boost::ref(executor.stream()),
                                                  _1 ));
    }
    catch (SystemException & ex) {
        if (! (ignoreMissing_ && ex.anyOf(ENOENT)))
            throw;
    }
}
#endif

//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ void senf::console::parseFile(std::string const & filename, DirectoryNode & root)
{
    ConfigFile cfg (filename, root);
    cfg.parse();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
