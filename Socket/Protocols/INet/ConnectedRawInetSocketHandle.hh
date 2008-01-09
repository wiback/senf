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

#ifndef CONNECTEDRAWINETSOCKETHANDLE_HH_
#define CONNECTEDRAWINETSOCKETHANDLE_HH_

// Custom includes
#include "INetProtocol.hh"
#include "RawInetProtocol.hh"
#include "../../../Socket/Protocols/BSDSocketProtocol.hh"
#include "../../../Socket/FramingPolicy.hh"
#include "../../../Socket/CommunicationPolicy.hh"
#include "../../../Socket/ReadWritePolicy.hh"
#include "../../../Socket/BufferingPolicy.hh"
#include "../../../Socket/ProtocolClientSocketHandle.hh"


///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup concrete_protocol_group
    /// @{

    typedef MakeSocketPolicy<
        INet4AddressingPolicy,
        DatagramFramingPolicy,
        ConnectedCommunicationPolicy,
        ReadablePolicy,
        WriteablePolicy,
        SocketBufferingPolicy
        >::policy ConnectedRawV4Socket_Policy;   ///< Socket Policy of the Connected RawV4 Protocol

    /** \brief IPv4 RAW Socket Protocol, connected

        \par Socket Handle typedefs:
            \ref ConnectedRawV4ClientSocketHandle (ProtocolClientSocketHandle)

        \par Policy Interface:
            ClientSocketHandle::read(), ClientSocketHandle::write(), ClientSocketHandle::bind(),
            ClientSocketHandle::local(), ClientSocketHandle::connect(), ClientSocketHandle::peer(),
            ClientSocketHandle::rcvbuf(), ClientSocketHandle::sndbuf()

        \par Address Type:
            INet4Address

        ConnectedRawV4SocketProtocol provides an internet protocol raw socket based on IPv4 addressing.
        This socket will put data written to it onto the IPv4 layer: if you call writeto don't inlude the header!
        On the other hand `read` will return the packet data including the IP header. 
        This behaviour is strange and differs from the behaviour of IPv6 RAW sockets and should be changed in the future. 

        This class is utilized as the protocol class of the ProtocolClientSocketHandle
        via the Socket Handle typedefs above.

        \see ConnectedRawV6SocketProtocol
        \see RawV4SocketProtocol
        \see RawV6SocketProtocol
     */
    class ConnectedRawV4SocketProtocol
        : public ConcreteSocketProtocol<ConnectedRawV4Socket_Policy>,
          public IPv4Protocol,
          public RawInetProtocol,
          public BSDSocketProtocol,
          public AddressableBSDSocketProtocol//,
//          public senf::pool_alloc_mixin<RawV4Socket_Policy>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // internal interface

        ///\name Constructors
        ///@{

        void init_client() const;       ///< Create unconnected client socket for IPPROTO_RAW
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */
        void init_client(int const & protocol) const;       ///< Create unconnected client socket for protocol
        
        void init_client(int const & protocol, INet4SocketAddress const & address) const;
                                        ///< Create client socket and connect
                                        /**< Creates a new client socket for the given protocol and connects to the given
                                             address.
                                             \param[in] protocol Layer 4 protocol to filter for / to send 
                                             \param[in] address local address to connect to */

        ///@}
        ///\name Abstract Interface Implementation

        std::auto_ptr<SocketProtocol> clone() const;

        ///@}
    };

    typedef ProtocolClientSocketHandle<ConnectedRawV4SocketProtocol> ConnectedRawV4ClientSocketHandle;




//////////////////////////////////////////////////////////////////// Raw IPv6 Socket //////////////////////////////////////
    typedef MakeSocketPolicy<
        INet6AddressingPolicy,
        DatagramFramingPolicy,
        ConnectedCommunicationPolicy,
        ReadablePolicy,
        WriteablePolicy,
        SocketBufferingPolicy
        >::policy ConnectedRawV6Socket_Policy;   ///< Socket Policy of the RawV6 Protocol

    /** \brief IPv6 RAW Socket Protocol, connected

        \par Socket Handle typedefs:
        \ref ConnectedRawV6ClientSocketHandle (ProtocolClientSocketHandle)

        \par Policy Interface: 
            ClientSocketHandle::read(), ClientSocketHandle::write(), ClientSocketHandle::bind(),
            ClientSocketHandle::local(), ClientSocketHandle::connect(), ClientSocketHandle::peer(),
            ClientSocketHandle::rcvbuf(), ClientSocketHandle::sndbuf()

        \par Address Type:
            INet6Address

        ConnectedRawV6SocketProtocol provides an internet protocol raw socket based on IPv6 addressing which is connected to certain peer.
        This socket will put data written to it onto the IPv6 layer: if you call writeto don't inlude the header!
        On the other hand `read` will return the packet data on top of the IPv6 layer, excluding the IP header. 
        Note: This behaviour is differs from the behaviour of IPv4 RAW sockets. 

        This class is utilized as the protocol class of the ProtocolClientSocketHandle
        via the Socket Handle typedefs above.

        \see ConnectedRawV4SocketProtocol
        \see RawV4SocketProtocol
        \see RawV6SocketProtocol
     */
    class ConnectedRawV6SocketProtocol
        : public ConcreteSocketProtocol<ConnectedRawV6Socket_Policy>,
          public IPv6Protocol,
          public RawInetProtocol,
          public BSDSocketProtocol,
          public AddressableBSDSocketProtocol//,
//          public senf::pool_alloc_mixin<RawV6SocketProtocol>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // internal interface

        ///\name Constructors
        ///@{

        void init_client() const;       ///< Create unconnected client socket for IPPROTO_RAW
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */

        void init_client(int const & protocol) const;       ///< Create unconnected client socket for protocol

        void init_client(int const & protocol, INet6SocketAddress const & address) const;
                                        ///< Create client socket and connect
                                        /**< Creates a new client socket for the given protocol and connects to the given
                                             address.
                                             \param[in] protocol Layer 4 protocol to filter for / to send 
                                             \param[in] address local address to connect to */
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor (??) */

        ///@}
        ///\name Abstract Interface Implementation

        std::auto_ptr<SocketProtocol> clone() const;

        ///@}
    };

    typedef ProtocolClientSocketHandle<ConnectedRawV6SocketProtocol> ConnectedRawV6ClientSocketHandle;

    /// @}

}

#endif /*CONNECTEDRAWINETSOCKETHANDLE_HH_*/
