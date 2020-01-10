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
    \brief UNDatagramSocketHandle public header */

#ifndef HH_SENF_Socket_Protocols_UN_UNDatagramSocketHandle_
#define HH_SENF_Socket_Protocols_UN_UNDatagramSocketHandle_ 1

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

    /// \addtogroup concrete_protocol_group
    //\{

    typedef MakeSocketPolicy<
        UNAddressingPolicy,
        DatagramFramingPolicy,
        UnconnectedCommunicationPolicy,
        ReadablePolicy,
        WriteablePolicy
        >::policy UNDatagramSocket_Policy;   ///< Socket Policy of the Unix Domain Datagram Protocol

    /** \brief Unix Domain Datagram Socket Protocol

        \par Socket Handle typedefs:
            \ref UNDatagramClientSocketHandle (ProtocolClientSocketHandle)

        \par Policy Interface: (to be checked)
            ClientSocketHandle::read(), ClientSocketHandle::readfrom(),
            ClientSocketHandle::writeto(), ClientSocketHandle::bind(), ClientSocketHandle::local()

        \par Address Type:
            UNSocketAddress

        UNDatagramSocketProtocol provides an datagram protocol socket based on the unix domain
        addressing.

        This class is utilized as the protocol class of the ProtocolClientSocketHandle via the
        Socket Handle typedefs above.
    */
    class UNDatagramSocketProtocol
        : public ConcreteSocketProtocol<UNDatagramSocket_Policy,
                                        UNDatagramSocketProtocol>,
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
                                        ///< Create client socket and bind
                                        /**< Creates a new client socket and bind to the given
                                             address.
                                             \param[in] address local address to bind to */
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */

        //\}
    };

    typedef ProtocolClientSocketHandle<UNDatagramSocketProtocol> UNDatagramClientSocketHandle;

    //\}

}
//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "UNDatagramSocketHandle.cci"
//#include "UNDatagramSocketHandle.ct"
//#include "UNDatagramSocketHandle.cti"
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
