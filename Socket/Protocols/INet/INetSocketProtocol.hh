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
    \brief IPv[46]Protocol public header

    \todo what about OOB data?

    \todo Implement IP_RECVERR / MSG_ERRQUEUE. This should be placed
    into an additional protocol facet since IP_RECVERR is only valid
    for SOCK_DGRAM (UDP) and not SOCK_STREAM (TCP) sockets
 */

#ifndef HH_INetSocketProtocol_
#define HH_INetSocketProtocol_ 1

// Custom includes
#include "../../../Socket/SocketProtocol.hh"
#include "INetAddressing.hh"
#include "../../../Socket/ClientSocketHandle.hh"
#include "../../../Socket/CommunicationPolicy.hh"

//#include "INetSocketProtocol.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup protocol_facets_group
    /// @{

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

    /// @}

}

///////////////////////////////hh.e////////////////////////////////////////
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
