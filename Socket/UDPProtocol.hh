// $Id$
//
// Copyright (C) 2007
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
    \brief UDPProtocol public header
 */

#ifndef HH_UDPProtocol_
#define HH_UDPProtocol_ 1

// Custom includes
#include "SocketProtocol.hh"
#include "INetAddressing.hh"

//#include "UDPProtocol.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup protocol_facets_group
    /// @{

    /** \brief Protocol facat to support UDP operations

        This protocol facet provides all those protocol functions,
        which are available on any UDP socket.
        
        \todo the multicast add/remove/iface semantics are quite unclear ...
     */
    class UDPProtocol
        : public virtual SocketProtocol
    {
    public:
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
                                                 (different from the sockaddress class */
        void mcAddMembership(INet4Address const & mcAddr, INet4Address const & localAddr) const;
                                        ///< join multicast group on a specific address/interface
                                        /**< This member will add \a mcAddr to the list of multicast
                                             groups received. The group is joined on the interface
                                             with the given local address.
                                             \param[in] mcAddr address of group to join
                                             \param[in] localAddr address of interface to join on
                                             \todo fix this as soon as we have a real address class
                                                 (different from the sockaddress class */

        void mcDropMembership(INet4Address const & mcAddr) const;
                                        ///< Leave multicast group
                                        /**< This member will remove \a mcAddr from the list of
                                             multicast groups received. The group is left from the
                                             default interface.
                                             \param[in] mcAddr address of group to leave
                                             \todo fix this as soon as we have a real address class
                                                 (different from the sockaddress class */
        void mcDropMembership(INet4Address const & mcAddr, INet4Address const & localAddr) const;
                                        ///< leave multicast group on a specific address/interface
                                        /**< This member will remove \a mcAddr from the list of
                                             multicast groups received. The group is left from the
                                             interface with the given local address.
                                             \param[in] mcAddr address of group to leave
                                             \param[in] localAddr address of interface to leave from
                                             \todo fix this as soon as we have a real address class
                                                 (different from the sockaddress class */

        void mcIface(std::string iface = std::string()) const;
                                        ///< set default multicast interface of the socket
                                        /**< \param[in] iface name of interface */
    

        ///\name Abstract Interface Implementation
        ///@{

        unsigned available() const;
        bool eof() const;

        ///@}
    };

    /// @}
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "UDPProtocol.cci"
//#include "UDPProtocol.ct"
//#include "UDPProtocol.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
