// $Id$
//
// Copyright (C) 2006 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

#ifndef HH_SocketPolicy_
#define HH_SocketPolicy_ 1

// Custom includes

//#include "SocketPolicy.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace lib {

    // This may be adapted to change the supported policies (however,
    // ClientSocketHandle and ServerSocketHandle will probably have to
    // be adjusted accordingly). However, AddressingPolicy MUST always
    // be the first Policy member ...

#   define SATLIB_SOCKET_POLICIES               \
	(AddressingPolicy)                      \
        (FramingPolicy)                         \
	(CommunicationPolicy)                   \
	(ReadPolicy)                            \
	(WritePolicy)                           \
	(BufferingPolicy)
    
    // The implementation file will for each Policy declared above
    // define the following (SomePolicy is one of the above):
    //
    // struct SomePolicyBase;
    // typedef UndefinedSomePolicy;
    // template SomePolicyIs< SocketPolicy, Axis >
    // template IfSomePolicyIs< SocketPolicy, Axis >
    // template IfSomePolicyIsNot< SocketPolicy, Axis >
    //
    // Additionally the following are defined:
    //
    // class SocketPolicyBase
    // template SocketPolicy< ..policies.. >
    // template MakeSocketPolicy< ..args.. >
    // template SocketPolicyIsBaseOf< Base, Derived >
}}

//////////////////////////////hh.e////////////////////////////////////////
#include "SocketPolicy.ih"
//#include "SocketPolicy.cci"
#include "SocketPolicy.ct"
//#include "SocketPolicy.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
