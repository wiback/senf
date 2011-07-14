// $Id$
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
    \brief LLSocketAddress and LLAddressingPolicy public header
 */

#ifndef HH_SENF_Socket_Protocols_Raw_LLAddressing_
#define HH_SENF_Socket_Protocols_Raw_LLAddressing_ 1

// Custom includes
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <senf/Socket/Protocols/BSDAddressingPolicy.hh>
#include <senf/Socket/Protocols/BSDSocketAddress.hh>
#include "MACAddress.hh"

//#include "LLAddressing.mpp"
//#include "LLAddressing.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /// \addtogroup addr_group
    //\{

    /** \brief Link local address

        LLSocketAddress wraps the standard sockaddr_ll data type. An LLSocketAddress provides quite
        some information, only part of which is necessary for sending packets. The LLSocketAddress
        class only allows changing those fields which need to be changed. The other fields are
        read-only. They are filled by the operating system when receiving a packet

        \nosubgrouping
     */
    class LLSocketAddress
        : public BSDSocketAddress
    {
    public:
        static short const addressFamily = AF_PACKET;

        /** \brief Valid pkttype() values

            These are the possible values returned by pkttype()
         */
        enum PktType { Undefined = 0
                     , Host      = PACKET_HOST      /**< Packet destined for this host */
                     , Broadcast = PACKET_BROADCAST /**< Packet sent to the broadcast address */
                     , Multicast = PACKET_MULTICAST /**< Packet sent to a (link local) multicast
                                                         address */
                     , OtherHost = PACKET_OTHERHOST /**< Packet sent to another host (promisc) */
                     , Outgoing  = PACKET_OUTGOING  /**< Packet sent out from this host */
        };

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        LLSocketAddress();              ///< Create empty address
        explicit LLSocketAddress(unsigned proto, std::string const & iface="");
                                        ///< Create address for \c bind()
                                        /**< This constructs an LLSocketAddress valid for calling
                                             PacketSocketHandle::bind() with.
                                             \param[in] proto Protocol (Ethertype) to listen for
                                             \param[in] iface Interface name to bind to */
        explicit LLSocketAddress(std::string const & iface);
                                        ///< Create address for \c bind()
                                        /**< This constructs an LLSocketAddress valid for calling
                                             \c PacketSocketHandle::bind() with.
                                             \param[in] iface Interface name to bind to
                                             \throws UnknownInterfaceException if \a iface is not
                                                 a valid interface name. */

        // This constructor is for sending packets
        explicit LLSocketAddress(MACAddress const & addr, std::string const & iface="");
                                        ///< Create address valid to send raw packets
                                        /**< Addresses created with this constructor are valid for
                                             use with \c PacketSocketHandle::sendto() on a \c
                                             SOCK_DGRAM packet socket.
                                             \param addr Address to send data to
                                             \param iface Interface to send packet from
                                             \throws UnknownInterfaceException if \a iface is not
                                                 a valid interface name. */

        LLSocketAddress(LLSocketAddress const & other);
        LLSocketAddress& operator=(LLSocketAddress const & other);

        //\}
        //-////////////////////////////////////////////////////////////////////////

        MACAddress address() const;     ///< Return address
        std::string interface() const;  ///< Return interface name
        unsigned protocol() const;      ///< Return address protocol (ethertype)

        unsigned arptype() const;       ///< Return the arptype field (ARP hardware type)
        PktType pkttype() const;        ///< Return type of packet

        // The mutating interface is purposely restricted to allow only
        // changing those members, which are sensible to be changed.

        void address(MACAddress const & addr); ///< Change address
        void interface(std::string const & iface); ///< Change interface
                                                   /**< \throws UnknownInterfaceException if \a iface
                                                             is not a valid interface name. */
        void protocol(unsigned prot);   ///< Change protocol

        using BSDSocketAddress::sockaddr_p;
        using BSDSocketAddress::socklen_p;

    private:
        struct ::sockaddr_ll addr_;
    };

    //\}

    /// \addtogroup policy_impl_group
    //\{

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
        : public BSDAddressingPolicy,
          private BSDAddressingPolicyMixin<LLSocketAddress>
    {
        typedef LLSocketAddress Address;

        using BSDAddressingPolicyMixin<LLSocketAddress>::local;
        using BSDAddressingPolicyMixin<LLSocketAddress>::bind;
    };

    //\}

    /** \brief Write link layer address
        \related LLSocketAddress
     */
    std::ostream & operator<<(std::ostream & os, LLSocketAddress const & llAddr);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
