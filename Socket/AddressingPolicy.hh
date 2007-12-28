// $Id$
//
// Copyright (C) 2006
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
    \brief NoAddressingPolicy public header
 */

#ifndef HH_AddressingPolicy_
#define HH_AddressingPolicy_ 1

// Custom includes
#include "SocketPolicy.hh"

//#include "AddressingPolicy.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup policy_impl_group
    /// @{

    /** \brief AddressingPolicy for non-addressable sockets

        This is different from UndefinedAddressingPolicy (which is the
        same as AddressingPolicyBase). This policy class defines the
        addressing -- it explicitly states, that the socket does not
        support any addressing.
     */
    struct NoAddressingPolicy : public AddressingPolicyBase
    {};

    /// @}
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "AddressingPolicy.cci"
//#include "AddressingPolicy.ct"
//#include "AddressingPolicy.cti"
//#include "AddressingPolicy.mpp"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
