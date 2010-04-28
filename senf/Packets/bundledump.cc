// $Id$
//
// Copyright (C) 2009 
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
    \brief bundledump non-inline non-template implementation */

// Custom includes
#include <dlfcn.h>
#include <iostream>
#include "Packets.hh"
#include <senf/Utils/Logger/Logger.hh>

//#include "bundledump.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

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

///////////////////////////////cc.e////////////////////////////////////////
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
