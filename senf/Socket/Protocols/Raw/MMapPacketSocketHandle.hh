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
#include <senf/Socket/QueueReadWritePolicy.hh>
#include <senf/Socket/Protocols/BSDSocketProtocol.hh>
#include <senf/Socket/Protocols/DatagramSocketProtocol.hh>
#include "MMapSocketProtocol.hh"
#include "LinuxPacketSocketProtocol.hh"
#include "LLAddressing.hh"

//#include "MMapPacketSocketHandle.mpp"
//#include "MMapPacketSocketHandle.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /// \addtogroup concrete_protocol_group
    //\{

    typedef MakeSocketPolicy<
        LLAddressingPolicy,
        DatagramFramingPolicy,
        ConnectedCommunicationPolicy,
        QueueReadPolicy,
        QueueWritePolicy
        >::policy ConnectedMMapPacket_Policy;        ///< Policy of ConnectedMMapPacketSocketProtocol

    /** \brief Raw Packet-Socket access using mmap read/write (Linux)

        \par Socket Handle typedefs:
        \ref ConnectedMMapPacketSocketHandle (ProtocolClientSocketHandle)

        \par Policy Interface:
        ClientSocketHandle::dequeue(), ClientSocketHandle::enqueue(),
        ClientSocketHandle::bind(), ClientSocketHandle::local()

        \par Address Type:
            LLSocketAddress

        The PacketSocketProtocol provides access to the linux packet socket API. This API gives access to
        the low level network packets. The packet socket allows read() and write() operations. The
        PacketSocketProtocol has no concept of a server socket.

        This class is utilized as the protocol class of the ProtocolClientSocketHandle via the
        Socket Handle typedefs above.

        This protocol uses the linux mmap packet socket access API. This API replaces the ordinary
        read/write calls with a linxu specific api.

        \warning The socket handle is neither readable nor writable in the ordinary sense. It
            utilises the special QueueReadPolicy and QueueWritePolicy socket policies.
     */
    class ConnectedMMapPacketSocketProtocol
        : public ConcreteSocketProtocol<ConnectedMMapPacket_Policy, ConnectedMMapPacketSocketProtocol>,
          public DatagramSocketProtocol,
          public BSDSocketProtocol,
          public MMapSocketProtocol,
          public LinuxPacketSocketProtocol
    {
    public:
        enum SocketType { RawSocket, DatagramSocket };
                                        ///< Socket types

        ///\name Constructors
        //\{
        void init_client(std::string iface, unsigned rxqlen, unsigned txqlen, unsigned frameSize=2048,
                         SocketType type = RawSocket, int protocol = -1) const;
                                        ///< Create packet socket
                                        /**< The new socket will receive all packets of the given
                                             IEEE 802.3 \a protocol. The socket will receive all
                                             packets, if \a protocol is -1.

                                             If \a type is \c RawSocket, the packet will include the
                                             link-level header (the Ethernet header). Sent packets
                                             must already include a well formed ll header.

                                             If \a type is \c DatagramSocket, the link level header
                                             will not be part of the packet data. The ll header will
                                             be removed from received packets and a correct ll
                                             header will be created on sent packets.

                                             \param[in] iface interface to bind to
                                             \param[in] rxqlen number of frames in rx queue
                                             \param[in] txqlen number of frames in tx queue
                                             \param[in] frameSize size of single frame in queue
                                             \param[in] type socket type
                                             \param[in] protocol IEEE 802.3 protocol number */
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */
        //\}


        ///\name Abstract Interface Implementation
        //\{

        unsigned available() const;
        bool eof() const;
        void close();
        void terminate() const;

        //\}

    };

    typedef ProtocolClientSocketHandle<ConnectedMMapPacketSocketProtocol> ConnectedMMapPacketSocketHandle;
                                        ///< SocketHandle of the ConnectedMMapPacketSocketProtocol
                                        /**< \related ConnectedMMapPacketPacketPrototol */

    //\}
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "MMapPacketSocketHandle.cci"
//#include "MMapPacketSocketHandle.ct"
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
