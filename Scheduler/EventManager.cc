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
    \brief EventManager non-inline non-template implementation */

//#include "EventManager.hh"
//#include "EventManager.ih"

// Custom includes
#include <boost/format.hpp>
#include "../Utils/membind.hh"
#include "Console/Console.hh"
#include "FIFORunner.hh"

//#include "EventManager.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::scheduler::detail::EventManager::EventManager()
{
#ifndef SENF_DISABLE_CONSOLE
    consoleDir_().add("events", senf::membind(&EventManager::consoleEvents, this));
    senf::console::sysdir().add("scheduler", consoleDir_());
#endif
}

prefix_ void senf::scheduler::detail::EventManager::consoleEvents(std::ostream & os)
{
    iterator i (begin());
    iterator const i_end (end());
    // On an 80 column display, this wraps nicely directly before the RUN column
    os << boost::format("%-6s %-52s %-10s %-8s %7s %s\n")
        % "TYPE"
        % "NAME"
        % "ADDRESS"
        % "ACTIVE?"
        % "RUN"
        % "INFO";
    for (; i != i_end; ++i)
        os << boost::format("%-6.6s %-52.52s 0x%08x %-8.8s %7d %s\n")
            % i->type()
            % i->name() 
            % reinterpret_cast<unsigned long>(&(*i))
            % (i->enabled() ? "enabled" : "disabled")
            % i->runCount()
            % i->info();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "EventManager.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
