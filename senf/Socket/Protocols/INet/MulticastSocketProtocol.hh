// $Id$
//
// Copyright (C) 2007
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
    \brief MulticastSocketProtocol public header */

#ifndef HH_SENF_Socket_Protocols_INet_MulticastSocketProtocol_
#define HH_SENF_Socket_Protocols_INet_MulticastSocketProtocol_ 1

// Custom includes
#include <senf/Socket/SocketProtocol.hh>
#include "INet4Address.hh"
#include "INet6Address.hh"

//#include "MulticastSocketProtocol.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    ///\addtogroup protocol_facets_group
    //\{

    /** \brief Generic addressing type independent multicast protocol facet

        \todo implement complete new multicast API from RFC3678 (as far as supported by linux)
        \bug mcLeaveSSMSource fails with EADDRNOTAVAIL
     */
    class MulticastSocketProtocol
        : public virtual SocketProtocol
    {
    public:
        void broadcastEnabled(bool v) const; ///< Enable broadcast send/receive
                                        /**< If this option is enabled, broadcast UDP messages will
                                             be received on the socket and the socket will be
                                             allowed to send out broadcast UDP messages
                                             \param[in] v \c true to enable broadcast send/receive,
                                                 \c false to disable */

        bool broadcastEnabled() const;  ///< Get broadcast send/receive state
                                        /**< \returns Current state of the broadcastEnabled()
                                             option. */


        unsigned mcTTL() const;         ///< Return current multicast TTL
        void mcTTL(unsigned value) const; ///< Set multicast TTL

        bool mcLoop() const;            ///< Return current multicast loopback state.
        void mcLoop(bool value) const;  ///< Set multicast loopback state
                                        /**< If set to false via \c mcLoop(value) multicast messages
                                             will not be looped back to local sockets. Default value
                                             is \c true. */

        void mcIface(std::string const & iface = std::string()) const;
                                        ///< Set multicast send interface of the socket
                                        /**< \param[in] iface name of interface to send multicast
                                             data from

                                             Under current linux versions this option is broken at
                                             best. Don't use. */
    };

    /** \brief Multicast protocol facet for INet4 addressable multicast enabled sockets
     */
    class INet4MulticastSocketProtocol
        : public MulticastSocketProtocol
    {
    public:
        void mcAddMembership(INet4Address const & mcAddr) const;
                                        ///< Join multicast group on default interface
                                        /**< This member will add \a mcAddr to the list of multicast
                                             groups received. The group is joined on the default
                                             interface.
                                             \param[in] mcAddr address of group to join */
        void mcAddMembership(INet4Address const & mcAddr, INet4Address const & localAddr) const;
                                        ///< join multicast group on a specific interface
                                        /**< This member will add \a mcAddr to the list of multicast
                                             groups received. The group is joined on the interface
                                             with the given local address.
                                             \deprecated Use \link mcAddMembership(INet4Address const & mcAddr, std::string const & iface) const mcAddMembership( \a mcAddr , \a iface )\endlink
                                                 for compatibility with the IPv6 API.
                                             \param[in] mcAddr address of group to join
                                             \param[in] localAddr address of interface to join on */
        void mcAddMembership(INet4Address const & mcAddr, std::string const & iface) const;
                                        ///< join multicast group on a specific interface
                                        /**< This member will add \a mcAddr to the list of multicast
                                             groups received. The group is joined on the given
                                             interface.
                                             \param[in] mcAddr address of group to join
                                             \param[in] iface interface name */

        void mcDropMembership(INet4Address const & mcAddr) const;
                                        ///< Leave multicast group
                                        /**< This member will remove \a mcAddr from the list of
                                             multicast groups received. The group is left from the
                                             default interface.
                                             \param[in] mcAddr address of group to leave */
        void mcDropMembership(INet4Address const & mcAddr, INet4Address const & localAddr)
            const;
                                        ///< leave multicast group on a specific interface
                                        /**< This member will remove \a mcAddr from the list of
                                             multicast groups received. The group is left from the
                                             interface with the given local address.
                                             \deprecated Use \link mcDropMembership(INet4Address const & mcAddr, std::string const & iface) const mcDropMembership( \a mcAddr , \a iface )\endlink
                                                 for compatibility with the IPv6 API.
                                             \param[in] mcAddr address of group to leave
                                             \param[in] localAddr address of interface to leave
                                                 from */
        void mcDropMembership(INet4Address const & mcAddr, std::string const & iface)
            const;
                                        ///< leave multicast group on a specific interface
                                        /**< This member will remove \a mcAddr from the list of
                                             multicast groups received. The group is left from the
                                             interface with the given local address.
                                             \param[in] mcAddr address of group to leave
                                             \param[in] iface interface name */

        void mcJoinSSMSource(INet4Address const & group, INet4Address const & source,
                             std::string const & iface) const;
                                        ///< join SSM multicast group
                                        /**< This call will join the multicast group \a group for
                                             traffic from \a source. A single group may be joined
                                             multiple times on different sources.
                                             \param[in] group multicast group to join
                                             \param[in] source SSM multicast source to join the
                                                 group on
                                             \param[in] iface interface to join the group on */
        void mcLeaveSSMSource(INet4Address const & group, INet4Address const & source,
                              std::string const & iface) const;
                                        ///< leave SSM multicast group
                                        /**< This call will leave the multicast group \a group for
                                             traffic from \a source.
                                             \param[in] group multicast group to leave
                                             \param[in] source SSM multicast source to leave the
                                                 group from
                                             \param[in] iface interface to leave the group on */
    };

    /** \brief Multicast protocol facet for INet6 addressable multicast enabled sockets

        This implementation supports INet6 mapped INet4 multicast addresses. This is a linux
        specific extension and NOT part of the relevant RFCs.
     */
    class INet6MulticastSocketProtocol
        : public MulticastSocketProtocol
    {
    public:
        void mcAddMembership(INet6Address const & mcAddr) const;
                                        ///< Join multicast group on default interface
                                        /**< This member will add \a mcAddr to the list of multicast
                                             groups received. The group is joined on the default
                                             interface.
                                             \param[in] mcAddr address of group to join */
        void mcAddMembership(INet6Address const & mcAddr, std::string const & iface) const;
                                        ///< join multicast group on a specific interface
                                        /**< This member will add \a mcAddr to the list of multicast
                                             groups received. The group is joined on the given
                                             interface.
                                             \param[in] mcAddr address of group to join
                                             \param[in] iface interface name */

        void mcDropMembership(INet6Address const & mcAddr) const;
                                        ///< Leave multicast group
                                        /**< This member will remove \a mcAddr from the list of
                                             multicast groups received. The group is left from the
                                             default interface.
                                             \param[in] mcAddr address of group to leave */
        void mcDropMembership(INet6Address const & mcAddr, std::string const & iface) const;
                                        ///< leave multicast group on a specific interface
                                        /**< This member will remove \a mcAddr from the list of
                                             multicast groups received. The group is left from the
                                             interface with the given local address.
                                             \param[in] mcAddr address of group to leave
                                             \param[in] iface interface name */

        void mcJoinSSMSource(INet6Address const & group, INet6Address const & source,
                             std::string const & iface) const;
                                        ///< join SSM multicast group
                                        /**< This call will join the multicast group \a group for
                                             traffic from \a source. A single group may be joined
                                             multiple times on different sources.
                                             \param[in] group multicast group to join
                                             \param[in] source SSM multicast source to join the
                                                 group on
                                             \param[in] iface interface to join the group on. If set
                                                 to the empty string, use the default interface. */
        void mcJoinSSMSource(INet6Address const & group, INet6Address const & source,
                             int ifacei = 0) const;
                                        ///< join SSM multicast group
                                        /**< This call will join the multicast group \a group for
                                             traffic from \a source. A single group may be joined
                                             multiple times on different sources.
                                             \param[in] group multicast group to join
                                             \param[in] source SSM multicast source to join the
                                                 group on
                                             \param[in] ifacei optional interface index to join the
                                                 group on */
        void mcLeaveSSMSource(INet6Address const & group, INet6Address const & source,
                              std::string const & iface) const;
                                        ///< leave SSM multicast group
                                        /**< This call will leave the multicast group \a group for
                                             traffic from \a source.
                                             \param[in] group multicast group to leave
                                             \param[in] source SSM multicast source to leave the
                                                 group from
                                             \param[in] iface interface to leave the group on */
    };

    //\}
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "MulticastSocketProtocol.cci"
//#include "MulticastSocketProtocol.ct"
//#include "MulticastSocketProtocol.cti"
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
