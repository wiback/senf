// $Id$
//
// Copyright (C) 2013 
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
    \brief LinuxPacketSocketProtocol public header */

#ifndef HH_SENF_senf_Socket_Protocols_Raw_LinuxPacketSocketProtocol_
#define HH_SENF_senf_Socket_Protocols_Raw_LinuxPacketSocketProtocol_ 1

// Custom includes
#include <senf/Socket/SocketProtocol.hh>
#include "MACAddress.hh"

//#include "LinuxPacketSocketProtocol.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    class LinuxPacketSocketProtocol
        : public virtual SocketProtocol
    {
    public:
        void mcAdd(std::string const & interface, MACAddress const & address) const;
                                        ///< Enable reception of a multicast group
                                        /**< mcAdd will join a new multicast group.

                                             \param[in] interface interface with which to join
                                             \param[in] address multicast address to join

                                             \see \ref LLSocketAddress */
        void mcDrop(std::string const & interface, MACAddress const & address) const;
                                        ///< Disable reception of a multicast group
                                        /**< \see \ref mcAdd() */

        void promisc(std::string const & interface, bool mode) const;
                                        ///< enable/disable promiscuous mode

        unsigned rxQueueDropped() const;
                                        ///< packets dropped by kernel since last call
                                        /**< query the number of packets dropped by the kernel since
                                             the last call to this method. */
    };
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "LinuxPacketSocketProtocol.cci"
//#include "LinuxPacketSocketProtocol.ct"
//#include "LinuxPacketSocketProtocol.cti"
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
