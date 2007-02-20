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
    \brief IPv[46]Protocol public header

    \todo what about OOB data?

    \todo Implement IP_RECVERR / MSG_ERRQUEUE. This should be placed
    into an additional protocol facet since IP_RECVERR is only valid
    for SOCK_DGRAM (UDP) and not SOCK_STREAM (TCP) sockets
 */

#ifndef HH_INetProtocol_
#define HH_INetProtocol_ 1

// Custom includes
#include "SocketProtocol.hh"
#include "INetAddressing.hh"
#include "ClientSocketHandle.hh"
#include "CommunicationPolicy.hh"

//#include "INetProtocol.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup protocol_facets_group
    /// @{

    /** \brief Protocol facet providing IPv4 Addressing related API

        This protocol facet introduces all the socket api protocol members which are related to IPv4
        addressing.

        \todo Is it safe, not to allow setting the interface index on add/drop? what does it do
        (especially if the local addres is given ?). What have I been thinking here ???

        \todo move all multicast-methods into an extra IPv4MulticastProtocol class (it's only
        available on datagram sockets)

        \todo the multicast add/remove/iface semantics are quite unclear ...

        \todo connect() is only available on stream sockets. We want to access bind() and connet()
        via the ClientSocketHandle -> see SocketProtocol todo point
     */
    class IPv4Protocol
        : public virtual SocketProtocol
    {
    public:
        void connect(INet4Address const & address) const; ///< Connect to remote address
                                        /**< \todo make this obsolete by allowing access to the
                                             ClientSocketHandle from ConcreateSocketProtocol
                                             \param[in] address Address to connect to */
        void bind(INet4Address const & address) const; ///< Set local socket address
                                        /**< \todo make this obsolete by allowing access to the
                                             ClientSocketHandle from ConcreateSocketProtocol
                                             \param[in] address Address to set */

        unsigned mcTTL() const;         ///< Return current multicast TTL
        void mcTTL(unsigned value) const; ///< Set multicast TTL

        bool mcLoop() const;            ///< Return current multicast loopback state
        void mcLoop(bool value) const;  ///< Set multicast loopback state

        void mcAddMembership(INet4Address const & mcAddr) const;
                                        ///< Join multicast group
                                        /**< This member will add \a mcAddr to the list of multicast
                                             groups received. The group is joined on the default
                                             interface.
                                             \param[in] mcAddr address of group to join
                                             \todo fix this as soon as we have a real address class
                                                 (differend from the sockaddress class */
        void mcAddMembership(INet4Address const & mcAddr, INet4Address const & localAddr) const;
                                        ///< join multicast group on a specific address/interface
                                        /**< This member will add \a mcAddr to the list of multicast
                                             groups received. The group is joined on the interface
                                             with the given local address.
                                             \param[in] mcAddr address of group to join
                                             \param[in] localAddr address of interface to join on
                                             \todo fix this as soon as we have a real address class
                                                 (differend from the sockaddress class */

        void mcDropMembership(INet4Address const & mcAddr) const;
                                        ///< Leave multicast group
                                        /**< This member will remove \a mcAddr from the list of
                                             multicast groups received. The group is left from the
                                             default interface.
                                             \param[in] mcAddr address of group to leave
                                             \todo fix this as soon as we have a real address class
                                                 (differend from the sockaddress class */
        void mcDropMembership(INet4Address const & mcAddr, INet4Address const & localAddr) const;
                                        ///< leave multicast group on a specific address/interface
                                        /**< This member will remove \a mcAddr from the list of
                                             multicast groups received. The group is left from the
                                             interface with the given local address.
                                             \param[in] mcAddr address of group to leave
                                             \param[in] localAddr address of interface to leave from
                                             \todo fix this as soon as we have a real address class
                                                 (differend from the sockaddress class */

        void mcIface(std::string iface = std::string()) const;
                                        ///< set default multicast interface of the socket
                                        /**< \param[in] iface name of interface */
    };

    /** \brief Protocol facet providing IPv6 Addressing related API

        This protocol facet introduces all the socket api protocol members which are related to IPv6
        addressing.
     */
    class IPv6Protocol
        : public virtual SocketProtocol
    {
    public:
        void connect(INet6SocketAddress const & address) const; ///< Connect to remote address
                                        /**< \todo make this obsolete by allowing access to the
                                             ClientSocketHandle from ConcreateSocketProtocol
                                             \param[in] address Address to connect to */
        void bind(INet6SocketAddress const & address) const; ///< Set local socket address
                                        /**< \todo make this obsolete by allowing access to the
                                             ClientSocketHandle from ConcreateSocketProtocol
                                             \param[in] address Address to set */
    };

    /// @}

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "INetProtocol.cci"
//#include "INetProtocol.ct"
//#include "INetProtocol.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
