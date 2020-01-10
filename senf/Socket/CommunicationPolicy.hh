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
    \brief CommunicationPolicy public header
 */

#ifndef HH_SENF_Socket_CommunicationPolicy_
#define HH_SENF_Socket_CommunicationPolicy_ 1

// Custom includes
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
