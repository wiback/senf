//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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

        unsigned txWrongFormat() const;
                                         ///< packets dropped by kernel due to wrong format (too large) since last call
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
