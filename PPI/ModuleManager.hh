// $Id$
//
// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief ModuleManager public header */

#ifndef HH_ModuleManager_
#define HH_ModuleManager_ 1

// Custom includes
#include <vector>
#include "predecl.hh"

//#include "ModuleManager.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {

    /** \brief Internal: Module management

        Every module is registered with the ModuleManager. The ModuleManager controls module
        initialization and execution of the network.
      */
    class ModuleManager
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        static ModuleManager & instance();

        // default default constructor
        // default copy constructor
        // default copy assignment
        // default destructor

        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        void init();                    ///< Called by senf::ppi::init()
        void run();                     ///< Called by senf::ppi::run()

        bool running() const;           ///< \c true, if the network is running

    private:
        ModuleManager();

        void registerModule(module::Module & module);
        void unregisterModule(module::Module & module);
        
        typedef std::vector<module::Module *> ModuleRegistry;

#ifndef DOXYGEN
        struct RunGuard;
        friend class RunGuard;
#endif

        ModuleRegistry moduleRegistry_;
        bool running_;
        bool terminate_;

        friend class module::Module;
    };


}}

///////////////////////////////hh.e////////////////////////////////////////
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
