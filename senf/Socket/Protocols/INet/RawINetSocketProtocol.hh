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


#ifndef HH_SENF_Socket_Protocols_INet_RawINetSocketProtocol_
#define HH_SENF_Socket_Protocols_INet_RawINetSocketProtocol_ 1

// Custom includes
#include "INetSocketProtocol.hh"

namespace senf {

    /// \addtogroup protocol_facets_group
    //\{

    /** \brief Protocol facet to support RAW operations upon IPv4/6

        This protocol facet provides all those protocol functions,
        which are available on a PF_INET[6]/RAW socket.

     */
    class RawINetSocketProtocol
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

#endif /*HH_SENF_Socket_Protocols_INet_RawINetSocketProtocol_*/
