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
    \brief DatagramSocketProtocol public header */

#ifndef HH_SENF_Socket_Protocols_DatagramSocketProtocol_
#define HH_SENF_Socket_Protocols_DatagramSocketProtocol_ 1

// Custom includes
#include <sys/time.h>
#include <senf/Socket/SocketProtocol.hh>
#include <senf/Scheduler/ClockService.hh>

//#include "DatagramSocketProtocol.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    ///\addtogroup protocol_facets_group
    //\{

    /** \brief Protocol facet providing generic BSD datagram socket functionality
      */
    class DatagramSocketProtocol
        : public virtual SocketProtocol
    {
    public:
        ClockService::clock_type timestamp_system() const; ///< Return packet timestamp of last packet

        ClockService::clock_type timestamp() const; ///< Return packet timestamp of last packet
                                        /**< The returned timestamp represents the time, at which
                                             the last network packet passed to the user has been
                                             received from the network. This allows precise network
                                             timing.

                                             \pre The \c SO_TIMESTAMP socket option must not be set
                                                 on the socket.
                                             \returns timestamp when last packet was received */
    };


    //\}
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
