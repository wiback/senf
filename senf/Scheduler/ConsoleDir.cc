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
    \brief Scheduler ConsoleDir non-inline non-template implementation */

#include "ConsoleDir.hh"
#include "ConsoleDir.ih"

// Custom includes
#include <senf/Utils/Console/ScopedDirectory.hh>
#include <senf/Utils/Console/Sysdir.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::console::ScopedDirectory<> & senf::scheduler::consoleDir()
{
    return detail::ConsoleDirManager::instance().consoleDir();
}

prefix_ senf::console::ScopedDirectory<> &
senf::scheduler::detail::ConsoleDirManager::consoleDir()
{
    return consoleDir_();
}

prefix_ senf::scheduler::detail::ConsoleDirManager::ConsoleDirManager()
{
    console::sysdir().add("scheduler", consoleDir_());
}

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
