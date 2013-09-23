// $Id$
//
// Copyright (C) 2013
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
    \brief LinuxPacketSocketProtocol public header */

#ifndef HH_SENF_Socket_Protocols_Raw_LinuxPacketSocketProtocol_
#define HH_SENF_Socket_Protocols_Raw_LinuxPacketSocketProtocol_ 1

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
        enum SocketType { RawSocket, DatagramSocket };
                                        ///< Socket types

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

        bool eof() const;

    protected:
        void init_packetSocket(SocketType type, int protocol) const;
    };

    class ReadableLinuxPacketProtocol
        : public virtual SocketProtocol
    {
    public:
        unsigned available() const;
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
