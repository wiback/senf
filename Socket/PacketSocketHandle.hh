// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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
 */

#ifndef HH_PacketSocketHandle_
#define HH_PacketSocketHandle_ 1

// Custom includes
#include "SocketPolicy.hh"
#include "SocketProtocol.hh"
#include "ProtocolClientSocketHandle.hh"
#include "LLAddressing.hh"
#include "FramingPolicy.hh"
#include "CommunicationPolicy.hh"
#include "ReadWritePolicy.hh"
#include "BufferingPolicy.hh"
#include "BSDSocketProtocol.hh"

//#include "PacketSocketHandle.mpp"
#include "PacketSocketHandle.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup concrete_protocol_group
    /// @{

    typedef MakeSocketPolicy<
        LLAddressingPolicy,
        DatagramFramingPolicy,
        UnconnectedCommunicationPolicy,
        ReadablePolicy,
        WriteablePolicy,
        SocketBufferingPolicy
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
          public BSDSocketProtocol
    {
    public:
        enum SocketType { RawSocket, DatagramSocket };
                                        ///< Socket types
        enum PromiscMode { Promiscuous, AllMulticast, None };
                                        ///< Interface modes

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
        void promisc(std::string interface, PromiscMode mode) const;
                                        ///< Change interface mode
                                        /**< This member will change the reception on the given
                                             interface. The modes available are

                                             <dl>
                                             <dt>\e None</dt><dd>No special mode set. Only receive
                                             packets addressed to the interface or of joined
                                             multicast groups</dd>
                                             <dt>\e AllMulticast</dt><dd>Additionally receive all
                                             multicast traffic</dd>
                                             <dt>\e Promiscuous</dt><dd>Receive all packets on the
                                             wire</dd>
                                             </dl>

                                             \param[in] interface interface to modify
                                             \param[in] mode new interface mode */

        // See LLSocketAddress for a discussion/rationale for ForwardRange here
        template <class ForwardRange>
        void mcAdd(std::string interface, ForwardRange const & address) const;
                                        ///< Enable reception of a multicast group
                                        /**< mcAdd will join a new multicast group. The address
                                             parameter is specified as an arbitrary forward range
                                             (see <a
                                             href="http://www.boost.org/libs/range/index.html">Boost.Range</a>)
                                             of up to 8 bytes. This  allows to initialize the
                                             address from an arbitrary sources without excessive
                                             copying.

                                             \param[in] interface interface with which to join
                                             \param[in] address multicast address to join

                                             \see \ref LLSocketAddress */
        template <class ForwardRange>
        void mcDrop(std::string interface, ForwardRange const & address) const;
                                        ///< Disable reception of a multicast group
                                        /**< \see \ref mcAdd() */
        ///@}

        ///\name Abstract Interface Implementation
        ///@{

        std::auto_ptr<SocketProtocol> clone() const;
        unsigned available() const;
        bool eof() const;

        ///@}

    private:
        template<class ForwardRange>
        void do_mc(std::string interface, ForwardRange const & address, bool add) const;
        void do_mc_i(std::string interface, detail::LLAddressCopier const & copier, bool add) const;
    };

    typedef ProtocolClientSocketHandle<PacketProtocol> PacketSocketHandle;
                                        ///< SocketHandle of the PacketProtocol
                                        /**< \related PacketPrototol */

    /// @}
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "PacketSocketHandle.cci"
#include "PacketSocketHandle.ct"
#include "PacketSocketHandle.cti"
//#include "PacketSocketHandle.mpp"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
