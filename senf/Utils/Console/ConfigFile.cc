//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
