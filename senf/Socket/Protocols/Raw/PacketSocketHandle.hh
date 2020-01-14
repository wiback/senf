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
    \brief PacketSocketProtocol and PacketSocketHandle public header
 */

#ifndef HH_SENF_Socket_Protocols_Raw_PacketSocketHandle_
#define HH_SENF_Socket_Protocols_Raw_PacketSocketHandle_ 1

// Custom includes
#include <senf/Socket/SocketPolicy.hh>
#include <senf/Socket/SocketProtocol.hh>
#include <senf/Socket/ProtocolClientSocketHandle.hh>
#include <senf/Socket/FramingPolicy.hh>
#include <senf/Socket/CommunicationPolicy.hh>
#include <senf/Socket/ReadWritePolicy.hh>
#include <senf/Socket/Protocols/BSDSocketProtocol.hh>
#include <senf/Socket/Protocols/DatagramSocketProtocol.hh>
#include "LinuxPacketSocketProtocol.hh"
#include "LLAddressing.hh"

//#include "PacketSocketHandle.mpp"
//#include "PacketSocketHandle.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /// \addtogroup concrete_protocol_group
    //\{

    typedef MakeSocketPolicy<
        LLAddressingPolicy,
        DatagramFramingPolicy,
        UnconnectedCommunicationPolicy,
        ReadablePolicy,
        WriteablePolicy
        >::policy Packet_Policy;        ///< Policy of PacketSocketProtocol

    /** \brief Raw Packet-Socket access (Linux)

        \par Socket Handle typedefs:
        \ref PacketSocketHandle (ProtocolClientSocketHandle)

        \par Policy Interface:
        ClientSocketHandle::read(), ClientSocketHandle::readfrom(), ClientSocketHandle::writeto(),
        ClientSocketHandle::bind(), ClientSocketHandle::local()

        \par Address Type:
            LLSocketAddress

        The PacketSocketProtocol provides access to the linux packet socket API. This API gives access to
        the low level network packets. The packet socket allows read() and write() operations. The
        PacketSocketProtocol has no concept of a server socket.

        This class is utilized as the protocol class of the ProtocolClientSocketHandle via the
        Socket Handle typedefs above.
     */
    class PacketSocketProtocol
        : public ConcreteSocketProtocol<Packet_Policy, PacketSocketProtocol>,
          public DatagramSocketProtocol,
          public BSDSocketProtocol,
          public LinuxPacketSocketProtocol,
          public ReadableLinuxPacketProtocol
    {
    public:
        ///\name Constructors
        //\{
        void init_client(SocketType type = RawSocket, int protocol = -1) const;
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

                                             \param[in] type socket type
                                             \param[in] protocol IEEE 802.3 protocol number */
                                        /**< \note This member is implicitly called from the
                                             ProtocolClientSocketHandle::ProtocolClientSocketHandle()
                                             constructor */
        //\}

    };

    typedef ProtocolClientSocketHandle<PacketSocketProtocol> PacketSocketHandle;
                                        ///< SocketHandle of the PacketSocketProtocol
                                        /**< \related PacketPrototol */

    //\}
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "PacketSocketHandle.cci"
//#include "PacketSocketHandle.ct"
//#include "PacketSocketHandle.cti"
//#include "PacketSocketHandle.mpp"
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
