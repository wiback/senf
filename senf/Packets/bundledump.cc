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
