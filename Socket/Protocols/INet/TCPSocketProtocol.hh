// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
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
    \brief TCPSocketProtocol public header
 */

#ifndef HH_TCPSocketProtocol_
#define HH_TCPSocketProtocol_ 1

// Custom includes
#include "../../../Socket/SocketProtocol.hh"
#include "INetSocketProtocol.hh"

//#include "TCPSocketProtocol.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup protocol_facets_group
    /// @{

    /** \brief Protocol facat to support TCP operations

        This protocol facet provides all those protocol functions,
        which are available on any TCP socket.
     */
    class TCPSocketProtocol
        : public virtual INetSocketProtocol
    {
    public:
        bool nodelay() const;           ///< Check current \c SO_NODELAY status
        void nodelay(bool value) const; ///< Set \c SO_NODELAY status
                                        /**< Enabling \c SO_NODELAY will disable the NAGLE
                                             algorithm (which aggregates multiple writes into a
                                             single network packet). Enabling nodelay() optimizes
                                             the repsonse time at the expense of the bandwidth
                                             efficiency.
                                             \param[in] value \c SO_NODELAY state */

        unsigned siocinq() const;       ///< Return current size of the input queue
        unsigned siocoutq() const;      ///< Return current size of the output queue

        enum ShutType { ShutRD, ShutWR, ShutRDWR };
        void shutdown(ShutType type) const;

        ///\name Abstract Interface Implementation
        ///@{

        unsigned available() const;
        bool eof() const;

        ///@}
    };

    /// @}
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "TCPSocketProtocol.cci"
//#include "TCPSocketProtocol.ct"
//#include "TCPSocketProtocol.cti"
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
