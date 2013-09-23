// $Id$
//
// Copyright (C) 2012
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
