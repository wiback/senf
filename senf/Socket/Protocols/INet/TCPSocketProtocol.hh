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
