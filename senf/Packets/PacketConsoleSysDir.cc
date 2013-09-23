// $Id$
//
// Copyright (C) 2012
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
