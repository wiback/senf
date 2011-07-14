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
    \brief ModuleManager public header */

#ifndef HH_SENF_PPI_ModuleManager_
#define HH_SENF_PPI_ModuleManager_ 1

// Custom includes
#include <vector>
#include <deque>
#include "predecl.hh"
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Utils/Console/ScopedDirectory.hh>

//#include "ModuleManager.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {

    /** \brief Internal: Module management

        Every module is registered with the ModuleManager. The ModuleManager controls module
        initialization and execution of the network.
      */
    class ModuleManager
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        struct Initializable
        {
            Initializable();
            virtual ~Initializable();
            ModuleManager & moduleManager() const;
            void enqueueInitializable();
            void dequeueInitializable();
            bool initializationScheduled() const;

            virtual void v_init() = 0;
        };

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        static ModuleManager & instance();

        // default default constructor
        // default copy constructor
        // default copy assignment
        // default destructor

        // no conversion constructors

        //\}
        //-////////////////////////////////////////////////////////////////////////

        void init();                    ///< Called by senf::ppi::init()
        void run();                     ///< Called by senf::ppi::run()

        bool running() const;           ///< \c true, if the network is running

        senf::console::DirectoryNode & consoleDir() const;

    private:
        ModuleManager();

        void registerModule(module::Module & module);
        void unregisterModule(module::Module & module);

        void registerInitializable(Initializable & i);
        void unregisterInitializable(Initializable & i);
        bool initializableRegistered(Initializable const & i) const;

        void dumpModules(std::ostream & os) const;

        typedef std::vector<module::Module *> ModuleRegistry;
        typedef std::deque<Initializable *> InitQueue;

#ifndef DOXYGEN
        struct RunGuard;
        friend class RunGuard;
#endif

        ModuleRegistry moduleRegistry_;
        bool running_;
        bool terminate_;

        InitQueue initQueue_;

        scheduler::EventHook initRunner_;

        senf::console::ScopedDirectory<> consoleDir_;

        friend class module::Module;
        friend class Initializable;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "ModuleManager.cci"
//#include "ModuleManager.ct"
//#include "ModuleManager.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
