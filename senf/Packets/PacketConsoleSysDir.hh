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
    \brief PacketConsoleSysDir public header */

#ifndef HH_SENF_Packets_PacketConsoleSysDir_
#define HH_SENF_Packets_PacketConsoleSysDir_ 1

#ifndef HH_SENF_Packets_Packets_
#error "Don't include 'PacketConsoleSysDir.hh' directly, include 'Packets.hh'"
#endif

// Custom includes
#include <senf/Utils/Console/LazyDirectory.hh>

//#include "PacketConsoleSysDir.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

#ifndef SENF_DISABLE_CONSOLE
    console::ScopedDirectory<> & packetConsoleDir();
#endif

}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(HH_SENF_Packets_Packets__decls_) && !defined(HH_SENF_Packets_PacketConsoleSysDir_i_)
#define HH_SENF_Packets_PacketConsoleSysDir_i_
//#include "PacketConsoleSysDir.cci"
//#include "PacketConsoleSysDir.ct"
//#include "PacketConsoleSysDir.cti"
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
