// $Id$
//
// Copyright (C) 2006
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
    \brief NoAddressingPolicy public header
 */

#ifndef HH_SENF_Socket_AddressingPolicy_
#define HH_SENF_Socket_AddressingPolicy_ 1

// Custom includes
#include "SocketPolicy.hh"

//#include "AddressingPolicy.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /// \addtogroup policy_impl_group
    //\{

    /** \brief AddressingPolicy for non-addressable sockets

        This is different from UndefinedAddressingPolicy (which is the
        same as AddressingPolicyBase). This policy class defines the
        addressing -- it explicitly states, that the socket does not
        support any addressing.
     */
    struct NoAddressingPolicy : public AddressingPolicyBase
    {};

    //\}
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
