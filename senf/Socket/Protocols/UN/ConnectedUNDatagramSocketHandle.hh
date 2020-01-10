//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief ConnectedUNDatagramSocketHandle public header */

#ifndef HHE_SENF_Socket_Protocols_UN_ConnectedUNDatagramSocketHandle_
#define HHE_SENF_Socket_Protocols_UN_ConnectedUNDatagramSocketHandle_

// Custom includes
#include "UNAddressing.hh"
#include "UNSocketProtocol.hh"
#include <senf/Socket/Protocols/BSDSocketProtocol.hh>
#include <senf/Socket/FramingPolicy.hh>
#include <senf/Socket/CommunicationPolicy.hh>
#include <senf/Socket/ReadWritePolicy.hh>
#include <senf/Socket/ProtocolClientSocketHandle.hh>

//#include "UNDatagramSocketHandle.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    typedef MakeSocketPolicy<
        UNAddressingPolicy,
        DatagramFramingPolicy,
        ConnectedCommunicationPolicy,
        ReadablePolicy,
        WriteablePolicy
        >::policy ConnectedUNDatagramSocket_Policy;   ///< Socket Policy of the Unix Domain Datagram Protocol (connected)

    /// \ingroup concrete_protocol_group
    //\{

    /** \brief Unix Domain Datagram Socket Protocol (connected)

        \par Socket Handle typedefs:
            \ref ConnectedUNDatagramClientSocketHandle (ProtocolClientSocketHandle)

        \par Policy Interface: (to be checked)
            ClientSocketHandle::read(), ClientSocketHandle::readfrom(),
            ClientSocketHandle::writeto(), ClientSocketHandle::bind(), ClientSocketHandle::local()

        \par Address Type:
            UNSocketAddress

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
        //-////////////////////////////////////////////////////////////////////////
        // internal interface

        ///\name Constructors
        //\{

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

        //\}
    };

    typedef ProtocolClientSocketHandle<ConnectedUNDatagramSocketProtocol> ConnectedUNDatagramClientSocketHandle;

    //\}

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
