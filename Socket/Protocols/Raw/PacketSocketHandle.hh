// $Id:PacketSocketHandle.hh 218 2007-03-20 14:39:32Z tho $
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS) 
// Competence Center NETwork research (NET), St. Augustin, GERMANY 
//     Stefan Bund <g0dil@berlios.de>
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

/** \file
    \brief PacketProtocol and PacketSocketHandle public header

    \todo Implement global promisc() helper based on ioctl() interface.
 */

#ifndef HH_PacketSocketHandle_
#define HH_PacketSocketHandle_ 1

// Custom includes
#include "../../../Socket/SocketPolicy.hh"
#include "../../../Socket/SocketProtocol.hh"
#include "../../../Socket/ProtocolClientSocketHandle.hh"
#include "../../../Socket/FramingPolicy.hh"
#include "../../../Socket/CommunicationPolicy.hh"
#include "../../../Socket/ReadWritePolicy.hh"
#include "../../../Socket/Protocols/BSDSocketProtocol.hh"
#include "LLAddressing.hh"

//#include "PacketSocketHandle.mpp"
//#include "PacketSocketHandle.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup concrete_protocol_group
    /// @{

    typedef MakeSocketPolicy<
        LLAddressingPolicy,
        DatagramFramingPolicy,
        UnconnectedCommunicationPolicy,
        ReadablePolicy,
        WriteablePolicy
        >::policy Packet_Policy;        ///< Policy of PacketProtocol

    /** \brief Raw Packet-Socket access (Linux)

        \par Socket Handle typedefs:
        \ref PacketSocketHandle (ProtocolClientSocketHandle)

        \par Policy Interface:
        ClientSocketHandle::read(), ClientSocketHandle::readfrom(), ClientSocketHandle::writeto(),
        ClientSocketHandle::bind(), ClientSocketHandle::local(), ClientSocketHandle::rcvbuf(),
        ClientSocketHandle::sndbuf()

        \par Address Type:
        LLSocketAddress

        The PacketProtocol provides access to the linux packet socket API. This API gives access to
        the low level network packets. The packet socket allows read() and write() operations. The
        PacketProtocol has no concept of a server socket.

        This class is utilized as the protocol class of the ProtocolClientSocketHandle via the
        Socket Handle typedefs above.
     */
    class PacketProtocol
        : public ConcreteSocketProtocol<Packet_Policy>,
          public BSDSocketProtocol,
          public senf::pool_alloc_mixin<PacketProtocol>
    {
    public:
        enum SocketType { RawSocket, DatagramSocket };
                                        ///< Socket types

        ///\name Constructors
        ///@{
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
        ///@}

        ///\name Protocol Interface
        ///@{

        void mcAdd(std::string const & interface, MACAddress const & address) const;
                                        ///< Enable reception of a multicast group
                                        /**< mcAdd will join a new multicast group.

                                             \param[in] interface interface with which to join
                                             \param[in] address multicast address to join

                                             \see \ref LLSocketAddress */
        void mcDrop(std::string const & interface, MACAddress const & address) const;
                                        ///< Disable reception of a multicast group
                                        /**< \see \ref mcAdd() */

        ///@}

        ///\name Abstract Interface Implementation
        ///@{

        std::auto_ptr<SocketProtocol> clone() const;
        unsigned available() const;
        bool eof() const;

        ///@}
    };

    typedef ProtocolClientSocketHandle<PacketProtocol> PacketSocketHandle;
                                        ///< SocketHandle of the PacketProtocol
                                        /**< \related PacketPrototol */

    /// @}
}

///////////////////////////////hh.e////////////////////////////////////////
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
