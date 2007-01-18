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

    \todo what about OOB data? 

    \todo Implement IP_RECVERR / MSG_ERRQUEUE. This should be placed
    into an additional protocol class since IP_RECVERR is only valid
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


    /** \brief

	\todo Is it safe, not to allow setting the interface index on
	add/drop? what does it do (especially if the local addres is
	given ?). What have I been thinking here ???
       
	\todo move all multicast-methods into an extra
	IPv4MulticastProtocol class
     */
    class IPv4Protocol 
        : public virtual SocketProtocol
    {
    public:
        void connect(INet4Address const & address) const;
        void bind(INet4Address const & address) const;

        unsigned mcTTL() const;
        void mcTTL(unsigned value) const;

        bool mcLoop() const;
        void mcLoop(bool value) const;

        void mcAddMembership(INet4Address const & mcAddr) const;
        void mcAddMembership(INet4Address const & mcAddr, INet4Address const & localAddr) const;

        void mcDropMembership(INet4Address const & mcAddr) const;
        void mcDropMembership(INet4Address const & mcAddr, INet4Address const & localAddr) const;

        void mcIface(std::string iface = std::string()) const;
    };
    
    class IPv6Protocol
        : public virtual SocketProtocol
    {};

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "INetProtocol.cci"
//#include "INetProtocol.ct"
//#include "INetProtocol.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "senf"
// End:
