// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum NETwork research (NET)
//     David Wagner <david.wagner@fokus.fraunhofer.de>
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
#ifndef RAWINETPROTOCOL_HH_
#define RAWINETPROTOCOL_HH_

// Custom includes
#include "INetAddressing.hh"
#include "INetProtocol.hh"
#include "../../../Socket/Protocols/BSDSocketProtocol.hh"
#include "../../../Socket/FramingPolicy.hh"
#include "../../../Socket/CommunicationPolicy.hh"
#include "../../../Socket/ReadWritePolicy.hh"
#include "../../../Socket/BufferingPolicy.hh"
#include "../../../Socket/ProtocolClientSocketHandle.hh"

namespace senf {

    /// \addtogroup protocol_facets_group
    /// @{

    /** \brief Protocol facat to support RAW operations upon IPv4/6 

        This protocol facet provides all those protocol functions,
        which are available on a PF_INET[6]/RAW socket.
        
     */
    class RawInetProtocol
        : public virtual SocketProtocol
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

#endif /*RAWINETPROTOCOL_HH_*/
