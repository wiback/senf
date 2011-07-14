// $Id$
//
// Copyright (C) 2006
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
