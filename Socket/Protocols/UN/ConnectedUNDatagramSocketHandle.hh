// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     David Wagner <dw6@berlios.de>
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
    \brief ConnectedUNDatagramSocketHandle public header */

#ifndef CONNECTEDUNDATAGRAMSOCKETHANDLE_HH_
#define CONNECTEDUNDATAGRAMSOCKETHANDLE_HH_

// Custom includes
#include "UNAddressing.hh"
#include "UNSocketProtocol.hh"
#include "../../../Socket/Protocols/BSDSocketProtocol.hh"
#include "../../../Socket/FramingPolicy.hh"
#include "../../../Socket/CommunicationPolicy.hh"
#include "../../../Socket/ReadWritePolicy.hh"
#include "../../../Socket/ProtocolClientSocketHandle.hh"

//#include "UNDatagramSocketHandle.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup concrete_protocol_group
    /// @{

    typedef MakeSocketPolicy<
        UNAddressingPolicy,
        DatagramFramingPolicy,
        ConnectedCommunicationPolicy,
        ReadablePolicy,
        WriteablePolicy
        >::policy ConnectedUNDatagramSocket_Policy;   ///< Socket Policy of the Unix Domain Datagram Protocol (connected)

    /** \brief Unix Domain Datagram Socket Protocol (connected)

        \par Socket Handle typedefs:
            \ref ConnectedUNDatagramClientSocketHandle (ProtocolClientSocketHandle)

        \par Policy Interface: (to be checked)
            ClientSocketHandle::read(), ClientSocketHandle::readfrom(),
            ClientSocketHandle::writeto(), ClientSocketHandle::bind(), ClientSocketHandle::local(),
            ClientSocketHandle::rcvbuf(), ClientSocketHandle::sndbuf()

        \par Address Type:
            UNAddress

        ConnectedUNDatagramSocketProtocol provides an datagram protocol socket based on the unix domain  addressing which operates in a connected mode.

        This class is utilized as the protocol class of the ProtocolClientSocketHandle
        via the Socket Handle typedefs above.

    */
    class ConnectedUNDatagramSocketProtocol
        : public ConcreteSocketProtocol<ConnectedUNDatagramSocket_Policy,
                                        ConnectedUNDatagramSocketProtocol>,
          public UNSocketProtocol, 
          public BSDSocketProtocol,
          public AddressableBSDSocketProtocol
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // internal interface

        ///\name Constructors
        ///@{

        void init_client() const;       ///< Create unconnected client socket
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */
        void init_client(UNSocketAddress const & address) const; 
                                        ///< Create client socket and connect
                                        /**< Creates a new client socket and connects to the given
                                             address.
                                             \param[in] address peer address to connect to */
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */
        
        ///@}
    };

    typedef ProtocolClientSocketHandle<ConnectedUNDatagramSocketProtocol> ConnectedUNDatagramClientSocketHandle;

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "ConnectedUNDatagramSocketHandle.cci"
//#include "ConnectedUNDatagramSocketHandle.ct"
//#include "ConnectedUNDatagramSocketHandle.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
