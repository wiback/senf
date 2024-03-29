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
    \brief UDPv4SocketHandle and UDPv6SocketHandle public header

    \todo Implement possibly non-blocking connect and SO_ERROR in the
    protocol interface
 */

#ifndef HH_SENF_Socket_Protocols_INet_ConnectedUDPSocketHandle_
#define HH_SENF_Socket_Protocols_INet_ConnectedUDPSocketHandle_ 1

// Custom includes
#include "INetAddressing.hh"
#include "UDPSocketProtocol.hh"
#include <senf/Socket/Protocols/BSDSocketProtocol.hh>
#include <senf/Socket/Protocols/DatagramSocketProtocol.hh>
#include <senf/Socket/FramingPolicy.hh>
#include <senf/Socket/CommunicationPolicy.hh>
#include <senf/Socket/ReadWritePolicy.hh>
#include <senf/Socket/ProtocolClientSocketHandle.hh>

//#include "ConnectedUDPSocketHandle.mpp"
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
        >::policy ConnectedUDPv4Socket_Policy;   ///< Socket Policy of the UDPv4 Protocol

    /** \brief IPv4 UDP Socket Protocol, connected

        \par Socket Handle typedefs:
            \ref ConnectedUDPv4ClientSocketHandle (ProtocolClientSocketHandle)

        \par Policy Interface:
            ClientSocketHandle::read(), ClientSocketHandle::write(), ClientSocketHandle::bind(),
            ClientSocketHandle::local(), ClientSocketHandle::connect(), ClientSocketHandle::peer()

        \par Address Type:
            INet4SocketAddress

        ConnectedUDPv4SocketProtocol provides an internet protocol stream socket based on the UDP
        protocol and IPv4 addressing.

        This class is utilized as the protocol class of the ProtocolClientSocketHandle
        via the Socket Handle typedefs above.

        \see ConnectedUDPv6SocketProtocol
     */
    class ConnectedUDPv4SocketProtocol
        : public ConcreteSocketProtocol<ConnectedUDPv4Socket_Policy, ConnectedUDPv4SocketProtocol>,
          public UDPSocketProtocol,
          public BSDSocketProtocol,
          public DatagramSocketProtocol,
          public AddressableBSDSocketProtocol
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // internal interface

        ///\name Constructors
        //\{

        void init_client() const;       ///< Create unconnected client socket
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */
        void init_client(INet4SocketAddress const & address) const;
                                        ///< Create client socket and connect
                                        /**< Creates a new client socket and connects to the given
                                             address.

                                             \param[in] address remote address to connect to */
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */

        //\}
    };

    typedef ProtocolClientSocketHandle<
        ConnectedUDPv4SocketProtocol> ConnectedUDPv4ClientSocketHandle;

    typedef MakeSocketPolicy<
        ConnectedUDPv4Socket_Policy,
        INet6AddressingPolicy
        >::policy  ConnectedUDPv6Socket_Policy;

    /** \brief IPv6 UDP Socket Protocol, connected

        \par Socket Handle typedefs:
            \ref ConnectedUDPv6ClientSocketHandle (ProtocolClientSocketHandle)

        \par Policy Interface:
            ClientSocketHandle::read(), ClientSocketHandle::write(), ClientSocketHandle::bind(),
            ClientSocketHandle::local(), ClientSocketHandle::connect(), ClientSocketHandle::peer()

        \par Address Type:
            INet6Address

        ConnectedUDPv6SocketProtocol provides an internet protocol stream socket based on the UDP
        protocol and IPv6 addressing.

        This class is utilized as the protocol class of the ProtocolClientSocketHandle
        via the Socket Handle typedefs above.

        \see ConnectedUDPv4SocketProtocol
     */
    class ConnectedUDPv6SocketProtocol
        : public ConcreteSocketProtocol<ConnectedUDPv6Socket_Policy, ConnectedUDPv6SocketProtocol>,
          public UDPSocketProtocol,
          public BSDSocketProtocol,
          public DatagramSocketProtocol,
          public AddressableBSDSocketProtocol
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // internal interface

        ///\name Constructors
        //\{

        void init_client() const;       ///< Create unconnected client socket
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */
        void init_client(INet6SocketAddress const & address) const;
                                        ///< Create client socket and connect
                                        /**< Creates a new client socket and connects to the given
                                             address.

                                             \param[in] address remote address to connect to */
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */

        //\}
    };

    typedef ProtocolClientSocketHandle<
        ConnectedUDPv6SocketProtocol> ConnectedUDPv6ClientSocketHandle;

    //\}

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "ConnectedUDPSocketHandle.cci"
//#include "ConnectedUDPSocketHandle.ct"
//#include "ConnectedUDPSocketHandle.cti"
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
