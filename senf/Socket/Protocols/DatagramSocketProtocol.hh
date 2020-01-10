//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
        void timestamp(struct timespec * spec) const; ///< Return packet timestamp of last packet

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
