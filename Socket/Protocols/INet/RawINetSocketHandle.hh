// $Id: RawINetSocketHandle.hh 597 2008-01-15 09:16:20Z g0dil $
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

#ifndef RAWINETSOCKETHANDLE_HH_
#define RAWINETSOCKETHANDLE_HH_


// Custom includes
#include "INetProtocol.hh"
#include "RawINetProtocol.hh"
#include "MulticastProtocol.hh"
#include "../../../Socket/Protocols/BSDSocketProtocol.hh"
#include "../../../Socket/FramingPolicy.hh"
#include "../../../Socket/CommunicationPolicy.hh"
#include "../../../Socket/ReadWritePolicy.hh"
#include "../../../Socket/ProtocolClientSocketHandle.hh"


///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup concrete_protocol_group
    /// @{

    typedef MakeSocketPolicy<
        INet4AddressingPolicy,
        DatagramFramingPolicy,
        UnconnectedCommunicationPolicy,
        ReadablePolicy,
        WriteablePolicy
        >::policy RawV4Socket_Policy;   ///< Socket Policy of the RawV4 Protocol

    /** \brief IPv4 RAW Socket Protocol

        \par Socket Handle typedefs:
            \ref RawV4ClientSocketHandle (ProtocolClientSocketHandle)

        \par Policy Interface:
            ClientSocketHandle::read(), ClientSocketHandle::readfrom(),
            ClientSocketHandle::writeto(), ClientSocketHandle::bind(), ClientSocketHandle::local(),
            ClientSocketHandle::rcvbuf(), ClientSocketHandle::sndbuf()

        \par Address Type:
            INet4Address

        RawV4SocketProtocol provides an internet protocol raw socket based on IPv4 addressing.
        This socket will put data written to it onto the IPv4 layer: if you call writeto don't inlude the header!
        On the other hand `read` will return the packet data including the IP header. 
        This behaviour is strange and differs from the behaviour of IPv6 RAW sockets and should be changed in the future. 

        This class is utilized as the protocol class of the ProtocolClientSocketHandle
        via the Socket Handle typedefs above.

        \see RawV6SocketProtocol
     */
    class RawV4SocketProtocol
        : public ConcreteSocketProtocol<RawV4Socket_Policy, RawV4SocketProtocol>,
          public RawINetProtocol,
          public BSDSocketProtocol,
          public AddressableBSDSocketProtocol,
          public MulticastProtocol,
          public INet4MulticastProtocol
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
                                        ///< Create client socket and bind
                                        /**< Creates a new client socket for the given protocol and binds to the given
                                             address.
                                             \param[in] protocol Layer 4 protocol to filter for / to send 
                                             \param[in] address local address to bind to */

        ///@}
    };

    typedef ProtocolClientSocketHandle<RawV4SocketProtocol> RawV4ClientSocketHandle;




//////////////////////////////////////////////////////////////////// Raw IPv6 Socket //////////////////////////////////////
    typedef MakeSocketPolicy<
        INet6AddressingPolicy,
        DatagramFramingPolicy,
        UnconnectedCommunicationPolicy,
        ReadablePolicy,
        WriteablePolicy
        >::policy RawV6Socket_Policy;   ///< Socket Policy of the RawV6 Protocol

    /** \brief IPv6 RAW Socket Protocol

        \par Socket Handle typedefs:
        \ref RawV6ClientSocketHandle (ProtocolClientSocketHandle)

        \par Policy Interface: 
            ClientSocketHandle::read(), ClientSocketHandle::readfrom(),
            ClientSocketHandle::writeto(), ClientSocketHandle::bind(), ClientSocketHandle::local(),
            ClientSocketHandle::rcvbuf(), ClientSocketHandle::sndbuf()

        \par Address Type:
            INet6Address

        RawV6SocketProtocol provides an internet protocol raw socket based on IPv6 addressing.
        This socket will put data written to it onto the IPv6 layer: if you call writeto don't inlude the header!
        On the other hand `read` will return the packet data on top of the IPv6 layer, excluding the IP header. 
        Note: This behaviour is differs from the behaviour of IPv4 RAW sockets. 

        This class is utilized as the protocol class of the ProtocolClientSocketHandle
        via the Socket Handle typedefs above.

        \see RawV4SocketProtocol
     */
    class RawV6SocketProtocol
        : public ConcreteSocketProtocol<RawV6Socket_Policy,RawV6SocketProtocol>,
          public RawINetProtocol,
          public BSDSocketProtocol,
          public AddressableBSDSocketProtocol,
          public MulticastProtocol,
          public INet4MulticastProtocol
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
                                        ///< Create client socket and bind
                                        /**< Creates a new client socket for the given protocol and binds to the given
                                             address.
                                             \param[in] protocol Layer 4 protocol to filter for / to send 
                                             \param[in] address local address to bind to */
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */

        ///@}
    };

    typedef ProtocolClientSocketHandle<RawV6SocketProtocol> RawV6ClientSocketHandle;

    /// @}

}

#endif /*RAWINETSOCKETHANDLE_HH_*/
