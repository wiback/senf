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
    \brief LLSocketAddress and LLAddressingPolicy public header
 */

#ifndef HH_LLAddressing_
#define HH_LLAddressing_ 1

// Custom includes
#include <sys/socket.h>
#include <netpacket/packet.h>
#include "Socket/SocketPolicy.hh"
#include "Socket/FileHandle.hh"
#include "Socket/Protocols/GenericAddressingPolicy.hh"
#include "MACAddress.hh"

//#include "LLAddressing.mpp"
//#include "LLAddressing.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup addr_group
    /// @{

    /** \brief Link local address

        LLSocketAddress wraps the standard sockaddr_ll data type. An LLSocketAddress provides quite
        some information, only part of which is necessary for sending packets. The LLSocketAddress
        class only allows changing those fields which need to be changed. The other fields are
        read-only. They are filled by the operating system when receiving a packet

     */
    class LLSocketAddress
    {
    public:
        /** \brief Valid pkttype() values

            These are the possible values returned by arptype() 
         */
        enum PktType { Undefined = 0
                     , Host      = PACKET_HOST      /**< Packet destined for this host */
                     , Broadcast = PACKET_BROADCAST /**< Packet sent to the broadcast address */
                     , Multicast = PACKET_MULTICAST /**< Packet sent to a (link local) multicast
                                                         address */
                     , OtherHost = PACKET_OTHERHOST /**< Packet sent to another host (promisc) */
                     , Outgoing  = PACKET_OUTGOING  /**< Packet sent out from this host */
        };
        
        LLSocketAddress();              ///< Create empty address
        explicit LLSocketAddress(unsigned proto, std::string const & iface="");
                                        ///< Create address for \c bind()
                                        /**< This constructs an LLSocketAddress valid for calling
                                             PacketSocketHandle::bind() with.
                                             \param[in] proto Protocol (Ethertype) to listen for
                                             \param[in] iface Interface name to bind to */
        explicit LLSocketAddress(std::string const &iface);
                                        ///< Create address for \c bind()
                                        /**< This constructs an LLSocketAddress valid for calling
                                             \c PacketSocketHandle::bind() with.
                                             \param[in] iface Interface name to bind to */

        // This constructor is for sending packets
        explicit LLSocketAddress(MACAddress const & addr, std::string const & iface="");
                                        ///< Create address valid to send raw packets
                                        /**< Addresses created with this constructor are valid for
                                             use with \c PacketSocketHandle::sendto() on a \c
                                             SOCK_DGRAM packet socket.
                                             \param addr Address to send data to
                                             \param iface Interface to send packet from */

        void clear();                   ///< Clear the address

        unsigned protocol() const;      ///< Return address protocol (ethertype)
        std::string interface() const;  ///< Return interface name
        unsigned arptype() const;       ///< Return the hatype field (ARP hardware type)
        PktType pkttype() const;        ///< Return type of packet
        MACAddress address() const;     ///< Return address

        // The mutating interface is purposely restricted to allow only
        // changing those members, which are sensible to be changed.

        void address(MACAddress const & addr); ///< Change address
        void interface(std::string const & iface); ///< Change interface
        void protocol(unsigned prot);   ///< Change protocol

        ///\name Generic SocketAddress interface
        ///@{

        struct sockaddr * sockaddr_p();
        struct sockaddr const * sockaddr_p() const;
        unsigned sockaddr_len() const;

        ///@}

    private:
        struct ::sockaddr_ll addr_;
    };

    /** \brief Signal invalid link local address syntax
        \related LLSocketAddress
     */
    struct InvalidLLSocketAddressException : public std::exception
    { char const * what() const throw() { return "invalid ll address"; } };

    /// @}

    /// \addtogroup policy_impl_group
    /// @{

    /** \brief Addressing policy supporting link-local addressing

        \par Address Type:
            LLSocketAddress

        This addressing policy implements generic link local
        addressing. The predominant type of link local addressing is
        Ethernet addressing.

        Since the link layer does not support the notion of
        connections, link local addresses do not support the connect()
        or peer() members.
     */
    struct LLAddressingPolicy
        : public AddressingPolicyBase,
          private GenericAddressingPolicy<LLSocketAddress>
    {
        typedef LLSocketAddress Address;

        using GenericAddressingPolicy<LLSocketAddress>::local;
        using GenericAddressingPolicy<LLSocketAddress>::bind;
    };

    /// @}
}

///////////////////////////////hh.e////////////////////////////////////////
#include "LLAddressing.cci"
//#include "LLAddressing.ct"
//#include "LLAddressing.cti"
//#include "LLAddressing.mpp"
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
