// $Id$
//
// Copyright (C) 2007 
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
    \brief DatagramSocketProtocol public header */

#ifndef HH_DatagramSocketProtocol_
#define HH_DatagramSocketProtocol_ 1

// Custom includes
#include <sys/time.h>
#include "../../Socket/SocketProtocol.hh"

//#include "DatagramSocketProtocol.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    ///\addtogroup protocol_facets_group
    ///\{

    /** \brief Protocol facet providing generic BSD datagram socket functionality
      */
    class DatagramSocketProtocol
        : public virtual SocketProtocol
    {
    public:
        struct timeval timestamp() const; ///< Return packet timestamp of last packet
                                        /**< The returned timestamp represents the time, at which
                                             the last network packet passed to the user has been
                                             received from the network. This allows precise network
                                             timing.
                                             \pre The \c SO_TIMESTAMP socket option must not be set
                                                 on the socket.
                                             \returns timestamp when last packet was received */
    };
    

    ///\}
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "DatagramSocketProtocol.cci"
//#include "DatagramSocketProtocol.ct"
//#include "DatagramSocketProtocol.cti"
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
