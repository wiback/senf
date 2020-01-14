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
    \brief PacketConsoleSysDir non-inline non-template implementation */

#include "Packets.hh"
#include "PacketConsoleSysDir.ih"

// Custom includes
#include <senf/Utils/Console/ScopedDirectory.hh>
#include <senf/Utils/Console/Sysdir.hh>

//#include "PacketConsoleSysDir.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

#ifndef SENF_DISABLE_CONSOLE

prefix_ senf::console::ScopedDirectory<> & senf::packetConsoleDir()
{
    return detail::packets::ConsoleDirManager::instance().consoleDir();
}

prefix_ senf::console::ScopedDirectory<> &
senf::detail::packets::ConsoleDirManager::consoleDir()
{
    return consoleDir_();
}

prefix_ senf::detail::packets::ConsoleDirManager::ConsoleDirManager()
{
    console::sysdir().add("packets", consoleDir_());
}

#endif

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "PacketConsoleSysDir.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
