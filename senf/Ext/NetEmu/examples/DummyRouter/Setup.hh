//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
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
