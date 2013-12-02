// $Id:PacketSocketHandle.hh 218 2007-03-20 14:39:32Z tho $
//
// Copyright (C) 2006
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
//   Stefan Bund <g0dil@berlios.de>

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
