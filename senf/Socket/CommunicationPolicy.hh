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
    \brief CommunicationPolicy public header
 */

#ifndef HH_SENF_Socket_CommunicationPolicy_
#define HH_SENF_Socket_CommunicationPolicy_ 1

// Custom includes
#include "SocketPolicy.hh"
#include "AddressingPolicy.hh"
#include "FileHandle.hh"

//#include "CommunicationPolicy.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

struct sockaddr;

namespace senf {

    /// \addtogroup policy_impl_group
    //\{

    template <class SPolicy> class ServerSocketHandle;

    /** \brief CommunicationPolicy for connected sockets

        The ConnectedCommunicationPolicy provides support for standard BSD socket API based
        connected communication. It provides the server side listen() and accept() members.
     */
    struct ConnectedCommunicationPolicy : public CommunicationPolicyBase
    {
#       ifndef DOXYGEN
        template <class SPolicy>
        static void listen(ServerSocketHandle<SPolicy> const & handle, unsigned backlog,
                           typename IfAddressingPolicyIsNot<SPolicy,NoAddressingPolicy>::type * = 0);
#       else
        template <class SPolicy>
        static void listen(ServerSocketHandle<SPolicy> const & handle, unsigned backlog);
                                        ///< Enable establishing new connections on the socket
                                        /**< \param[in] handle socket handle to enable reception on
                                             \param[in] backlog size of backlog queue */
#       endif

#       ifndef DOXYGEN
        template <class SPolicy>
        static int accept(ServerSocketHandle<SPolicy> const & handle,
                          typename ServerSocketHandle<SPolicy>::Address & address,
                          typename IfAddressingPolicyIsNot<SPolicy,NoAddressingPolicy>::type * = 0);
#       else
        template <class SPolicy>
        static int accept(ServerSocketHandle<SPolicy> const & handle,
                          typename ServerSocketHandle<SPolicy>::Address & address);
                                        ///< accept a new connection on the socket.
                                        /**< The accept() member will return a new client file
                                             descriptor. This file descriptor will be used by the
                                             ServerSocketHandle implementation to build a new
                                             ClientSocketHandle for the new connection.

                                             \param[in] handle socket handle to accept connection on
                                             \param[out] address address of newly connected remote
                                                 peer
                                             \returns file descriptor of new client socket */
#       endif

        static int accept(FileHandle const & handle);
                                        ///< accept a new connection on the socket.
                                        /**< The accept() member will return a new client file
                                             descriptor. This file descriptor will be used by the
                                             ServerSocketHandle implementation to build a new
                                             ClientSocketHandle for the new connection.

                                             \param[in] handle socket handle to accept connection on
                                             \returns file descriptor of new client socket */

    private:
        static void do_listen(FileHandle const & handle, unsigned backlog);
        static int do_accept(FileHandle const & handle, struct sockaddr * addr, unsigned len);
    };

    /** \brief CommunicationPolicy for unconnected sockets

        This is different from UndefinedCommunicationPolicy (which is the same as
        CommunicationPolicyBase). This policy class defines the communication policy -- it
        explicitly states, that the socket does not support connected communication. This
        effectively disables the ServerSocketHandle.
     */
    struct UnconnectedCommunicationPolicy : public CommunicationPolicyBase
    {};

    //\}

}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "CommunicationPolicy.cci"
//#include "CommunicationPolicy.ct"
#include "CommunicationPolicy.cti"
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
