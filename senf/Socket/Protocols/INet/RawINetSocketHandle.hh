// $Id: RawINetSocketHandle.hh 597 2008-01-15 09:16:20Z g0dil $
//
// Copyright (C) 2007
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
//   David Wagner <dw6@berlios.de>


#ifndef HH_SENF_Socket_Protocols_INet_RawINetSocketHandle_
#define HH_SENF_Socket_Protocols_INet_RawINetSocketHandle_ 1


// Custom includes
#include "INetAddressing.hh"
#include "RawINetSocketProtocol.hh"
#include "MulticastSocketProtocol.hh"
#include <senf/Socket/Protocols/BSDSocketProtocol.hh>
#include <senf/Socket/Protocols/DatagramSocketProtocol.hh>
#include <senf/Socket/FramingPolicy.hh>
#include <senf/Socket/CommunicationPolicy.hh>
#include <senf/Socket/ReadWritePolicy.hh>
#include <senf/Socket/ProtocolClientSocketHandle.hh>


//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /// \addtogroup concrete_protocol_group
    //\{

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
            ClientSocketHandle::writeto(), ClientSocketHandle::bind(), ClientSocketHandle::local()

        \par Address Type:
            INet4SocketAddress

        RawV4SocketProtocol provides an internet protocol raw socket based on IPv4 addressing.
        This socket will put data written to it onto the IPv4 layer: if you call writeto don't include the header!
        On the other hand `read` will return the packet data including the IP header.
        This behaviour is strange and differs from the behaviour of IPv6 RAW sockets and should be changed in the future.

        This class is utilized as the protocol class of the ProtocolClientSocketHandle
        via the Socket Handle typedefs above.

        \attention
            If socket handle with RawV4SocketProtocol is connected via INet4SocketAddress, the port number
            is interpreted as protocol number for IPv4 layer. Please refer manpage: "man 7 raw".

        \see RawV6SocketProtocol
     */
    class RawV4SocketProtocol
        : public ConcreteSocketProtocol<RawV4Socket_Policy, RawV4SocketProtocol>,
          public RawINetSocketProtocol,
          public BSDSocketProtocol,
          public AddressableBSDSocketProtocol,
          public DatagramSocketProtocol,
          public INet4MulticastSocketProtocol
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // internal interface

        ///\name Constructors
        //\{

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

        //\}
    };

    typedef ProtocolClientSocketHandle<RawV4SocketProtocol> RawV4ClientSocketHandle;




//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
            ClientSocketHandle::writeto(), ClientSocketHandle::bind(), ClientSocketHandle::local()

        \par Address Type:
            INet6Address

        RawV6SocketProtocol provides an internet protocol raw socket based on IPv6 addressing.
        This socket will put data written to it onto the IPv6 layer: if you call writeto don't include the header!
        On the other hand `read` will return the packet data on top of the IPv6 layer, excluding the IP header.
        Note: This behaviour is differs from the behaviour of IPv4 RAW sockets.

        This class is utilized as the protocol class of the ProtocolClientSocketHandle
        via the Socket Handle typedefs above.

        \attention
            If socket handle with ConnectedRawV6SocketProtocol is connected via INet6SocketAddress, the port number
            is interpreted as protocol number for IPv6 layer. Please refer manpage: "man 7 ipv6".

        \see RawV4SocketProtocol
     */
    class RawV6SocketProtocol
        : public ConcreteSocketProtocol<RawV6Socket_Policy,RawV6SocketProtocol>,
          public RawINetSocketProtocol,
          public BSDSocketProtocol,
          public AddressableBSDSocketProtocol,
          public DatagramSocketProtocol,
          public INet6MulticastSocketProtocol
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // internal interface

        ///\name Constructors
        //\{

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

        //\}
    };

    typedef ProtocolClientSocketHandle<RawV6SocketProtocol> RawV6ClientSocketHandle;

    //\}

}

#endif /*HH_SENF_Socket_Protocols_INet_RawINetSocketHandle_*/
