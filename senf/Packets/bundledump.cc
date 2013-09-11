// $Id$
//
// Copyright (C) 2009
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
    \brief bundledump non-inline non-template implementation */

// Custom includes
#include <dlfcn.h>
#include <iostream>
#include <senf/Utils/Logger/Logger.hh>
#include "Packets.hh"

//#include "bundledump.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char const ** argv)
{
    // Link in logger library ...
    senf::IGNORE( senf::log::StreamRegistry::instance() );
    for (int i (1); i<argc; ++i) {
        senf::detail::PacketRegistryImplBase::clear();
        void *handle = dlopen(argv[i], RTLD_NOW | RTLD_GLOBAL);
        if (handle == NULL) {
            std::cerr << "could not load packet bundle " << argv[i] << ": "
                      << dlerror() << std::endl;
            return 1;
        }
    }
    senf::dumpPacketRegistries(std::cout);
    return 0;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "bundledump.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
