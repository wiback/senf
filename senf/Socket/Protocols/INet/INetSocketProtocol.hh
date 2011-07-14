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
    \brief IPv[46]Protocol public header

    \todo what about OOB data?

    \todo Implement IP_RECVERR / MSG_ERRQUEUE. This should be placed
    into an additional protocol facet since IP_RECVERR is only valid
    for SOCK_DGRAM (UDP) and not SOCK_STREAM (TCP) sockets
 */

#ifndef HH_SENF_Socket_Protocols_INet_INetSocketProtocol_
#define HH_SENF_Socket_Protocols_INet_INetSocketProtocol_ 1

// Custom includes
#include <senf/Socket/SocketProtocol.hh>

//#include "INetSocketProtocol.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /// \addtogroup protocol_facets_group
    //\{

    /** \brief Generic addressing type independent INet protocol facet
     */
    class INetSocketProtocol
        : public virtual SocketProtocol
    {
    public:
        void bindInterface(std::string const & iface) const;
                                        ///< Bind socket to specific interface
                                        /**< When a socket is bound to an interface, it will only
                                             receive data received on that interface. If the
                                             interface name is empty, the binding is removed.

                                             \param[in] iface name of interface to bind to or empty
                                                 to remove binding

                                             \implementation Sets the SO_BINDTODEVICE socket option
                                          */
        std::string bindInterface();    ///< Get bound interface
                                        /**< Returns the interface, the socket is currently bound
                                             to. Returns the empty string, if not bound to any
                                             interface.
                                             \returns Bound interface name */
    };

    //\}

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "INetSocketProtocol.cci"
//#include "INetSocketProtocol.ct"
//#include "INetSocketProtocol.cti"
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
