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
