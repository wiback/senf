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
    \brief PacketSocketProtocol and MMapPacketSocketHandle public header
 */

#ifndef HH_SENF_Socket_Protocols_Raw_MMapPacketSocketHandle_
#define HH_SENF_Socket_Protocols_Raw_MMapPacketSocketHandle_ 1

// Custom includes
#include <senf/Socket/SocketPolicy.hh>
#include <senf/Socket/SocketProtocol.hh>
#include <senf/Socket/ProtocolClientSocketHandle.hh>
#include <senf/Socket/FramingPolicy.hh>
#include <senf/Socket/CommunicationPolicy.hh>
#include <senf/Socket/ReadWritePolicy.hh>
#include <senf/Socket/QueueReadWritePolicy.hh>
#include <senf/Socket/Protocols/BSDSocketProtocol.hh>
#include <senf/Socket/Protocols/DatagramSocketProtocol.hh>
#include "MMapSocketProtocol.hh"
#include "LinuxPacketSocketProtocol.hh"
#include "LLAddressing.hh"

//#include "MMapPacketSocketHandle.mpp"
#include "MMapPacketSocketHandle.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /// \addtogroup concrete_protocol_group
    //\{

    typedef MakeSocketPolicy<
        LLAddressingPolicy,
        DatagramFramingPolicy,
        ConnectedCommunicationPolicy,
        ReadablePolicy,
        WriteablePolicy
        >::policy ConnectedMMapPacket_Policy; ///< Policy of ConnectedMMapPacketSocketProtocol

    /** \brief Raw Packet-Socket access using mmap read/write (Linux)

        \par Socket Handle typedefs:
        \ref ConnectedMMapPacketSocketHandle (ProtocolClientSocketHandle)

        \par Policy Interface:
        ClientSocketHandle::dequeue(), ClientSocketHandle::enqueue(),
        ClientSocketHandle::bind(), ClientSocketHandle::local()

        \par Address Type:
            LLSocketAddress

        The PacketSocketProtocol provides access to the linux packet socket API. This API gives
        access to the low level network packets. The packet socket allows read() and write()
        operations. The PacketSocketProtocol has no concept of a server socket.

        This class is utilized as the protocol class of the ProtocolClientSocketHandle via the
        Socket Handle typedefs above.

        This protocol uses the linux mmap packet socket access API. This API replaces the ordinary
        read/write calls with a linux specific api.

        \warning The socket handle is neither readable nor writable in the ordinary sense. It
            utilises the special QueueReadPolicy and QueueWritePolicy socket policies.
     */
    template <class P1=mpl::nil, class P2=mpl::nil>
    class ConnectedMMapPacketSocketProtocol
        : public ConcreteSocketProtocol<
              typename MakeSocketPolicy<ConnectedMMapPacket_Policy, P1, P2>::policy,
              ConnectedMMapPacketSocketProtocol<P1, P2> >,
          public detail::ConnectedMMapPacketSocketProtocol_Bases<
              typename MakeSocketPolicy<ConnectedMMapPacket_Policy, P1, P2>::policy>
    {
    public:
        typedef typename MakeSocketPolicy<ConnectedMMapPacket_Policy, P1, P2>::policy Policy;
        typedef typename detail::ConnectedMMapPacketSocketProtocol_Bases<
            typename MakeSocketPolicy<ConnectedMMapPacket_Policy, P1, P2>::policy> Base;

    private:
    };

    typedef ProtocolClientSocketHandle<
        ConnectedMMapPacketSocketProtocol<QueueReadPolicy, QueueWritePolicy> >
        ConnectedMMapPacketSocketHandle;
                                        ///< SocketHandle of the ConnectedMMapPacketSocketProtocol
                                        /**< \related ConnectedMMapPacketPacketPrototol */

    typedef ProtocolClientSocketHandle<
        ConnectedMMapPacketSocketProtocol<QueueReadPolicy> >
        ConnectedMMapReadPacketSocketHandle;
                                        ///< SocketHandle of the ConnectedMMapPacketSocketProtocol
                                        /**< \related ConnectedMMapPacketPacketPrototol */

    typedef ProtocolClientSocketHandle<
        ConnectedMMapPacketSocketProtocol<QueueWritePolicy> >
        ConnectedMMapWritePacketSocketHandle;
                                        ///< SocketHandle of the ConnectedMMapPacketSocketProtocol
                                        /**< \related ConnectedMMapPacketPacketPrototol */

    typedef ProtocolClientSocketHandle<
        ConnectedMMapPacketSocketProtocol<> >
        ConnectedPacketSocketHandle;
                                        ///< SocketHandle of the ConnectedMMapPacketSocketProtocol
                                        /**< \related ConnectedMMapPacketPacketPrototol */

    //\}
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "MMapPacketSocketHandle.cci"
#include "MMapPacketSocketHandle.ct"
//#include "MMapPacketSocketHandle.cti"
//#include "MMapPacketSocketHandle.mpp"
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
