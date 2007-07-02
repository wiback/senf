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
    \brief FramingPolicy public header
 */

#ifndef HH_FramingPolicy_
#define HH_FramingPolicy_ 1

// Custom includes
#include "SocketPolicy.hh"

//#include "FramingPolicy.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup policy_impl_group
    /// @{

    /** \brief FramingPolicy for stream oriented sockets

        This policy does not explicitly modify the SocketHAndle
        API. It however affects the semantics of the read and write
        operations. On a stream oriented socket, read() and write()
        operations may be combined, the boundary between separate
        write() calls will be lost on the receiving side.
     */
    struct StreamFramingPolicy : public FramingPolicyBase
    {};

    /** \brief FramingPolicy for datagram oriented sockets

        This policy does not explicitly modify the SocketHAndle
        API. It however affects the semantics of the read and write
        operations. On a datagram socket, each read() or write() call
        we read or write a single datagram. Datagram boundaries are
        kept intact accross the network.
     */
    struct DatagramFramingPolicy : public FramingPolicyBase
    {};

    /// @}

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "FramingPolicy.cci"
//#include "FramingPolicy.ct"
//#include "FramingPolicy.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
