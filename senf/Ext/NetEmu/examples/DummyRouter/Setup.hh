// $Id:$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// http://senf.fit.fraunhofer.de/license.html
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

/** \file
    \brief Setup public header */

#ifndef HH_CARMEN_Setup_
#define HH_CARMEN_Setup_ 1

// Custom includes
#include <string>
#include "Config.hh"
#include <senf/Ext/NetEmu/Annotations.hh>
#include <senf/Ext/NetEmu/InterfaceAPI.hh>
#include <boost/ptr_container/ptr_vector.hpp>

//#include "Setup.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace wiback {

    /** \brief Emulator setup

        This class manages the external framework of the CARMEN emulation: The (emulated) network
        interfaces and the external routing modules needed. It has one output and one input: Here
        the CARMEN routing module will be connected.
     */
    template <class Decoration>
    class Setup
    {
    private:
        senf::ppi::module::PassiveJoin<> join_;
        senf::ppi::module::AnnotationRouter<senf::emu::annotations::Interface> director_;

        typedef senf::emu::Interface::Decorator Interface;
        typedef boost::ptr_vector<Interface> Interfaces;

    public:
        typedef boost::iterator_range<Interfaces::iterator> InterfaceRange;

        senf::ppi::connector::ActiveOutput<> & output;
        senf::ppi::connector::PassiveInput<> & input;

        Setup();

        template <class IfaceType>
        void registerInterfaceType(std::string const & name);
        template <class IfaceType, class Arg>
        void registerInterfaceType(std::string const & name, std::string const & arg,
                                   std::string doc);

        template <class Config>
        void init(Config & config);

        InterfaceRange interfaces();

    private:
        void addInterface(std::unique_ptr<Interface> iface);

        template <class IfaceType>
        senf::console::DirectoryNode::ptr createInterface();
        template <class IfaceType, class Arg>
        senf::console::DirectoryNode::ptr createInterface(Arg const & arg);

        Interfaces interfaces_;
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "Setup.cci"
#include "Setup.ct"
#include "Setup.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -U"
// End:
