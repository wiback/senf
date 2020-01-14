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
    \brief TCPSocketProtocol public header
 */

#ifndef HH_SENF_Socket_Protocols_INet_TCPSocketProtocol_
#define HH_SENF_Socket_Protocols_INet_TCPSocketProtocol_ 1

// Custom includes
#include "INetSocketProtocol.hh"

//#include "TCPSocketProtocol.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /// \addtogroup protocol_facets_group
    //\{

    /** \brief Protocol facet to support TCP operations

        This protocol facet provides all those protocol functions,
        which are available on any TCP socket.
     */
    class TCPSocketProtocol
        : public virtual INetSocketProtocol
    {
    public:
        bool nodelay() const;           ///< Check current \c SO_NODELAY status
        void nodelay(bool value) const; ///< Set \c SO_NODELAY status
                                        /**< Enabling \c SO_NODELAY will disable the NAGLE
                                             algorithm (which aggregates multiple writes into a
                                             single network packet). Enabling nodelay() optimizes
                                             the response time at the expense of the bandwidth
                                             efficiency.
                                             \param[in] value \c SO_NODELAY state */

        unsigned siocinq() const;       ///< Return current size of the input queue
        unsigned siocoutq() const;      ///< Return current size of the output queue

        enum ShutType { ShutRD, ShutWR, ShutRDWR };
        void shutdown(ShutType type) const;

        ///\name Abstract Interface Implementation
        //\{

        void close();
        unsigned available() const;
        bool eof() const;

        //\}
    };

    //\}
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "TCPSocketProtocol.cci"
//#include "TCPSocketProtocol.ct"
//#include "TCPSocketProtocol.cti"
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
