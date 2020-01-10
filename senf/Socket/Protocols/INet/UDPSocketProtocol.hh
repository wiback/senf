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
    \brief UDPSocketProtocol public header
 */

#ifndef HH_SENF_Socket_Protocols_INet_UDPSocketProtocol_
#define HH_SENF_Socket_Protocols_INet_UDPSocketProtocol_ 1

// Custom includes
#include "INetSocketProtocol.hh"

//#include "UDPSocketProtocol.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /// \addtogroup protocol_facets_group
    //\{

    /** \brief Protocol facet to support UDP operations

        This protocol facet provides all those protocol functions,
        which are available on any UDP socket.
     */
    class UDPSocketProtocol
        : public virtual INetSocketProtocol
    {
    public:
        ///\name Abstract Interface Implementation
        //\{

        unsigned available() const;
        bool eof() const;

        //\}
    };

    //\}
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "UDPSocketProtocol.cci"
//#include "UDPSocketProtocol.ct"
//#include "UDPSocketProtocol.cti"
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
