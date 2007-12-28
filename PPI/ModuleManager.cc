// $Id$
//
// Copyright (C) 2007 
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
    \brief ModuleManager non-inline non-template implementation */

#include "ModuleManager.hh"
//#include "ModuleManager.ih"

// Custom includes
#include "../Scheduler/Scheduler.hh"
#include "Module.hh"

//#include "ModuleManager.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::ppi::ModuleManager

prefix_ void senf::ppi::ModuleManager::init()
{
    ModuleRegistry::const_iterator i (moduleRegistry_.begin());
    ModuleRegistry::const_iterator const i_end (moduleRegistry_.end());
    for (; i != i_end; ++i)
        (*i)->init();
}

#ifndef DOXYGEN

struct senf::ppi::ModuleManager::RunGuard
{
    RunGuard(ModuleManager & m) : manager(m) { manager.running_ = true; }
    ~RunGuard() { manager.running_ = false; }
    ModuleManager & manager;
};

#endif

prefix_ void senf::ppi::ModuleManager::run()
{
    init();
    RunGuard guard (*this);
    Scheduler::instance().process();
}

////////////////////////////////////////
// private members

prefix_ senf::ppi::ModuleManager::ModuleManager()
    : running_(false), terminate_(false)
{}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "ModuleManager.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
