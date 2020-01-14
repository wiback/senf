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
    \brief ConsoleTarget non-inline non-template implementation */

#include "ConsoleTarget.hh"

// Custom includes
#include <iostream>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::log::ConsoleTarget

prefix_ senf::log::ConsoleTarget & senf::log::ConsoleTarget::instance()
{
    static ConsoleTarget consoleTarget;
    return consoleTarget;
}

prefix_ void senf::log::ConsoleTarget::logToStderr()
{
    instance().stream(std::cerr);
}

prefix_ senf::log::ConsoleTarget::ConsoleTarget()
    : IOStreamTarget("console", std::cout)
{}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
