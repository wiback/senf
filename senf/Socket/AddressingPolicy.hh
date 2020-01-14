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
