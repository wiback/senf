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
    \brief ModuleManager public header */

#ifndef HH_SENF_PPI_ModuleManager_
#define HH_SENF_PPI_ModuleManager_ 1

// Custom includes
#include <vector>
#include <deque>
#include "predecl.hh"
#include <senf/Scheduler/EventHook.hh>
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

        console::DirectoryNode & consoleDir() const;

    private:
        ModuleManager();
        ~ModuleManager();

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
        friend struct RunGuard;
#endif

        ModuleRegistry moduleRegistry_;
        bool running_;
        bool alive_;

        InitQueue initQueue_;

        scheduler::EventHook initRunner_;

        console::ScopedDirectory<> consoleDir_;

        friend class module::Module;
        friend struct Initializable;
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
