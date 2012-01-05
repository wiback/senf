// $Id$
//
// Copyright (C) 2012
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
