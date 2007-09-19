// $Id$
//
// Copyright (C) 2007 
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
    \brief UNProtocol public header */

#ifndef HH_UNProtocol_
#define HH_UNProtocol_ 1

// Custom includes
#include "../../../Socket/SocketProtocol.hh"
#include "UNAddressing.hh"
#include "../../../Socket/ClientSocketHandle.hh"
#include "../../../Socket/CommunicationPolicy.hh"

//#include "UNProtocol.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup protocol_facets_group
    /// @{

    /** \brief Protocol facet providing Unix Domain Addressing related API

        This protocol facet introduces all the socket api protocol members which are related to Unix 
        Domain addressing.

        \todo connect() is only available on stream sockets. We want to access bind() and connect()
        via the ClientSocketHandle -> see SocketProtocol todo point
     */
    class UNProtocol
        : public virtual SocketProtocol
    {
    public:
        void connect(UNSocketAddress const & address) const; ///< Connect to a  unix domain socket 
                                        /**< \todo make this obsolete by allowing access to the
                                             ClientSocketHandle from ConcreateSocketProtocol
                                             \param[in] address Address to connect to */
        void bind(UNSocketAddress const & address) const; ///< Set local socket address (path)
                                        /**< \todo make this obsolete by allowing access to the
                                             ClientSocketHandle from ConcreateSocketProtocol
                                             \param[in] address Address to set */
    };
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "UNProtocol.cci"
//#include "UNProtocol.ct"
//#include "UNProtocol.cti"
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
