// $Id: ConnectedRawINetSocketHandle.hh 597 2008-01-15 09:16:20Z g0dil $
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

#ifndef CONNECTEDHH_SENF_Socket_Protocols_INet_RawINetSocketHandle_
#define CONNECTEDHH_SENF_Socket_Protocols_INet_RawINetSocketHandle_

// Custom includes
#include "INetAddressing.hh"
#include "RawINetSocketProtocol.hh"
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
        ConnectedCommunicationPolicy,
        ReadablePolicy,
        WriteablePolicy
        >::policy ConnectedRawV4Socket_Policy;   ///< Socket Policy of the Connected RawV4 Protocol

    /** \brief IPv4 RAW Socket Protocol, connected

        \par Socket Handle typedefs:
            \ref ConnectedRawV4ClientSocketHandle (ProtocolClientSocketHandle)

        \par Policy Interface:
            ClientSocketHandle::read(), ClientSocketHandle::write(), ClientSocketHandle::bind(),
            ClientSocketHandle::local(), ClientSocketHandle::connect(), ClientSocketHandle::peer()

        \par Address Type:
            INet4SocketAddress

        ConnectedRawV4SocketProtocol provides an Internet protocol raw socket based on IPv4
        addressing. This socket will put data written to it onto the IPv4 layer: if you call
        writeto don't include the header!

        On the other hand `read` will return the packet data including the IP header.
        This behaviour is strange and differs from the behaviour of IPv6 RAW sockets and should be
        changed in the future.

        This class is utilized as the protocol class of the ProtocolClientSocketHandle
        via the Socket Handle typedefs above.

        \attention If socket handle with ConnectedRawV4SocketProtocol is connected via
            INet4SocketAddress, the port number is interpreted as protocol number for IPv4
            layer. Please refer manpage: "man 7 raw".

        \see ConnectedRawV6SocketProtocol
        \see RawV4SocketProtocol
        \see RawV6SocketProtocol
     */
    class ConnectedRawV4SocketProtocol
        : public ConcreteSocketProtocol<ConnectedRawV4Socket_Policy, ConnectedRawV4SocketProtocol>,
          public RawINetSocketProtocol,
          public BSDSocketProtocol,
          public DatagramSocketProtocol,
          public AddressableBSDSocketProtocol
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
        void init_client(int const & protocol) const;
                                        ///< Create unconnected client socket for protocol

        void init_client(int const & protocol, INet4SocketAddress const & address) const;
                                        ///< Create client socket and connect
                                        /**< Creates a new client socket for the given protocol and
                                             connects to the given address.
                                             \param[in] protocol Layer 4 protocol to filter for / to
                                                 send
                                             \param[in] address local address to connect to */

        //\}
    };

    typedef ProtocolClientSocketHandle<ConnectedRawV4SocketProtocol>
        ConnectedRawV4ClientSocketHandle;




//-/////////////////////////////////////////////////////////////////////////////////////////////////
    typedef MakeSocketPolicy<
        INet6AddressingPolicy,
        DatagramFramingPolicy,
        ConnectedCommunicationPolicy,
        ReadablePolicy,
        WriteablePolicy
        >::policy ConnectedRawV6Socket_Policy;   ///< Socket Policy of the RawV6 Protocol

    /** \brief IPv6 RAW Socket Protocol, connected

        \par Socket Handle typedefs:
        \ref ConnectedRawV6ClientSocketHandle (ProtocolClientSocketHandle)

        \par Policy Interface:
            ClientSocketHandle::read(), ClientSocketHandle::write(), ClientSocketHandle::bind(),
            ClientSocketHandle::local(), ClientSocketHandle::connect(), ClientSocketHandle::peer()

        \par Address Type:
            INet6Address

        ConnectedRawV6SocketProtocol provides an internet protocol raw socket based on IPv6
        addressing which is connected to certain peer.  This socket will put data written to it onto
        the IPv6 layer: if you call writeto don't include the header!  On the other hand `read` will
        return the packet data on top of the IPv6 layer, excluding the IP header.  Note: This
        behaviour is differs from the behaviour of IPv4 RAW sockets.

        This class is utilized as the protocol class of the ProtocolClientSocketHandle
        via the Socket Handle typedefs above.

        \attention If socket handle with ConnectedRawV6SocketProtocol is connected via
            INet6SocketAddress, the port number is interpreted as protocol number for IPv6
            layer. Please refer manpage: "man 7 ipv6".

        \see ConnectedRawV4SocketProtocol
        \see RawV4SocketProtocol
        \see RawV6SocketProtocol
     */
    class ConnectedRawV6SocketProtocol
        : public ConcreteSocketProtocol<ConnectedRawV6Socket_Policy, ConnectedRawV6SocketProtocol>,
          public RawINetSocketProtocol,
          public BSDSocketProtocol,
          public DatagramSocketProtocol,
          public AddressableBSDSocketProtocol
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

        void init_client(int const & protocol) const;
                                        ///< Create unconnected client socket for protocol

        void init_client(int const & protocol, INet6SocketAddress const & address) const;
                                        ///< Create client socket and connect
                                        /**< Creates a new client socket for the given protocol and
                                             connects to the given address.
                                             \param[in] protocol Layer 4 protocol to filter for / to
                                                 send
                                             \param[in] address local address to connect to
                                             \note This member is implicitly called from the //
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor (??) */

        //\}
    };

    typedef ProtocolClientSocketHandle<ConnectedRawV6SocketProtocol>
        ConnectedRawV6ClientSocketHandle;

    //\}

}

#endif /*CONNECTEDHH_SENF_Socket_Protocols_INet_RawINetSocketHandle_*/


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
