// $Id$
//
// Copyright (C) 2007
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
    \brief ModuleManager non-inline non-template implementation */

#include "ModuleManager.hh"
//#include "ModuleManager.ih"

// Custom includes
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Utils/membind.hh>
#include <senf/Utils/Console/ParsedCommand.hh>
#include <senf/Utils/Console/Sysdir.hh>
#include "Module.hh"

//#include "ModuleManager.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// private members

prefix_ void senf::ppi::ModuleManager::registerInitializable(Initializable & i)
{
    initQueue_.push_back(&i);
    initRunner_.enable();
    // This call ensures, that the senf::ppi::init() handler is called as next handler
    // after this handler returns (this works since the senf::ppi::init() handler is registered as
    // PRE hook and thus has very high priority)
    senf::scheduler::yield();
}

prefix_ void senf::ppi::ModuleManager::unregisterInitializable(Initializable & i)
{
    initQueue_.erase(
        std::remove(initQueue_.begin(), initQueue_.end(), & i),
        initQueue_.end());
    if (initQueue_.empty())
        initRunner_.disable();
}

prefix_ senf::ppi::ModuleManager::ModuleManager()
    : running_(false), terminate_(false),
      initRunner_ ("senf::ppi::init", membind(&ModuleManager::init, this),
                   scheduler::EventHook::PRE, false)
{
    console::sysdir().add("ppi", consoleDir_);

    consoleDir_
        .add("dump", console::factory::Command(
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
    consoleDir_.add("modules", console::factory::Directory());
}

prefix_ void senf::ppi::ModuleManager::registerModule(module::Module & module)
{
    moduleRegistry_.push_back(&module);
}

prefix_ void senf::ppi::ModuleManager::unregisterModule(module::Module & module)
{
    moduleRegistry_.erase(
        std::remove(moduleRegistry_.begin(), moduleRegistry_.end(), & module),
        moduleRegistry_.end());
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
