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
    \brief UDPSocketProtocol public header
 */

#ifndef HH_SENF_Socket_Protocols_INet_UDPSocketProtocol_
#define HH_SENF_Socket_Protocols_INet_UDPSocketProtocol_ 1

// Custom includes
#include <senf/Socket/SocketProtocol.hh>
#include "INetAddressing.hh"
#include "INetSocketProtocol.hh"

//#include "UDPSocketProtocol.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup protocol_facets_group
    /// @{

    /** \brief Protocol facat to support UDP operations

        This protocol facet provides all those protocol functions,
        which are available on any UDP socket.
     */
    class UDPSocketProtocol
        : public virtual INetSocketProtocol
    {
    public:
        ///\name Abstract Interface Implementation
        ///@{

        unsigned available() const;
        bool eof() const;

        ///@}
    };

    /// @}
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "UDPSocketProtocol.cci"
//#include "UDPSocketProtocol.ct"
//#include "UDPSocketProtocol.cti"
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
