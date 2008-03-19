// $Id$
//
// Copyright (C) 2008 
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
    \brief Server public header */

#ifndef HH_Server_
#define HH_Server_ 1

// Custom includes
#include "../Socket/Protocols/INet/INetAddressing.hh"

//#include "Server.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace console {

    void start(senf::INet4SocketAddress const & address);
    void start(senf::INet6SocketAddress const & address);

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "Server.cci"
//#include "Server.ct"
//#include "Server.cti"
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
