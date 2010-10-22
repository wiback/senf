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
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Utils/membind.hh>
#include "Module.hh"
#include <senf/Utils/Console/ParsedCommand.hh>
#include <senf/Utils/Console/Sysdir.hh>

//#include "ModuleManager.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::ppi::ModuleManager

prefix_ void senf::ppi::ModuleManager::init()
{
    while (! initQueue_.empty()) {
        initQueue_.front()->v_init();
        initQueue_.pop_front();
    }
    initRunner_.disable();
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
    scheduler::process();
}

////////////////////////////////////////
// private members

prefix_ senf::ppi::ModuleManager::ModuleManager()
    : running_(false), terminate_(false),
      initRunner_ ("senf::ppi::init", membind(&ModuleManager::init, this),
                   scheduler::EventHook::PRE, false)
{
    senf::console::sysdir().add("ppi", consoleDir_);

    consoleDir_
        .add("dump", senf::console::factory::Command(
                 senf::membind(&ModuleManager::dumpModules, this))
             .doc("Dump complete PPI structure\n"
                  "The dump will contain one paragraph for each module. The first line gives module\n"
                  "information, additional lines list all connectors and their peers (if connected).\n"
                  "\n"
                  "This information can be processed by 'tools/drawmodules.py' and 'dot' (from the\n"
                  "graphviz package) to generate a graphic representation of the module structure:\n"
                  "\n"
                  "    $ echo /sys/ppi/dump | nc -q1 <host> <port> \\\n"
                  "          | python tools/drawmodules.py | dot -Tpng /dev/fd/0 >modules.png\n")
            );
}

prefix_ void senf::ppi::ModuleManager::dumpModules(std::ostream & os)
    const
{
    for (ModuleRegistry::const_iterator i (moduleRegistry_.begin()), i_end (moduleRegistry_.end());
         i != i_end; ++i) {
        os << *i << " " << prettyName(typeid(**i)) << "\n";
        for (module::Module::ConnectorRegistry::iterator j ((*i)->connectorRegistry_.begin()),
                 j_end ((*i)->connectorRegistry_.end()); j != j_end; ++j) {
            os << "  " << *j << " " << prettyName(typeid(**j));
            if ((**j).connected()) {
                os << " " << & (*j)->peer();
                connector::PassiveConnector * pc (dynamic_cast<connector::PassiveConnector *>(*j));
                if (pc && pc->throttled())
                    os << " throttled";
            }
            os << "\n";
        }
        os << "\n";
    }
}

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
