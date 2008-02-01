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
    \brief GenericAddressingPolicy public header
 */

/** \defgroup addr_group Addressing classes
 */

#ifndef HH_GenericAddressingPolicy_
#define HH_GenericAddressingPolicy_ 1

// Custom includes
#include "../../Socket/SocketHandle.hh"
#include "../../Socket/FileHandle.hh"
#include "../../Socket/SocketPolicy.hh"
#include "../../Socket/CommunicationPolicy.hh"

//#include "GenericAddressingPolicy.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup policy_impl_group
    /// @{

    /** \brief Non-template implementation class of GenericAddressingPolicy template

        \internal
     */
    struct GenericAddressingPolicy_Base
    {
        static void do_local(FileHandle handle, struct sockaddr * addr, unsigned len);
        static void do_peer(FileHandle handle, struct sockaddr * addr, unsigned len);
        static void do_bind(FileHandle handle, struct sockaddr const * addr, unsigned len);
        static void do_connect(FileHandle handle, struct sockaddr const * addr, unsigned len);
    };

    /** \brief Template for generic AddressingPolicy implementation based on the BSD socket API

        This template provides an implementation template to implement generic addressing policy
        classes which rely on the standard BSD socket API for their implementation
        (connect/bind/getsockname/getpeername).

        The \a Address template parameter specifies the address type of the addressing policy. This
        type must have two members: \c sockaddr_p() and \c sockaddr_len(). The first must return a
        <tt>struct sockaddr *</tt> to the address, the second must return the size of the address in
        bytes. The pointer returned by \c sockaddr_p() must be non-const if called on a non-const
        address. <em>The underlying socket address stored at that pointer might be
        modified</em>.

        This template class is inherited into addressing policy classes via private
        inheritance. Then the members supported by the respective addressing policy are made
        available via \c using declarations (See INet4AddressingPolicy for an Example).

        \idea We could explicitly provide open_sockaddr_p() and close_sockaddr_p()
        members. sockaddr_p could always return a const * whereas open_sockaddr_p should return a
        non-const pointer. The close operation would then explicitly signal, that the new value
        should be incorporated into the class. With our current implementation, the close member
        would be a no-op, however this should free us from using the sockaddr values as a direct
        storage representation of the address.
     */
    template <class Address>
    struct GenericAddressingPolicy
        : private GenericAddressingPolicy_Base
    {
#       ifndef DOXYGEN
        template <class SPolicy>
        static void peer(SocketHandle<SPolicy> handle, Address & addr,
                         typename IfCommunicationPolicyIs<SPolicy,ConnectedCommunicationPolicy>::type * = 0);
#       else
        template <class SPolicy>
        static void peer(SocketHandle<SPolicy> handle, Address & addr);
                                        ///< Return address of remote peer on connected sockets
                                        /**< This member is only available if the socket handles
                                             communication policy is ConnectedCommunicationPolicy.

                                             \param[in] handle socket handle to get peer address of
                                             \param[out] addr address of remote peer */
#       endif
        static void local(FileHandle handle, Address & addr);
                                        ///< Return local of socket
                                        /**< \param[in] handle socket handle to check
                                             \param[out] addr local socket address */

#       ifndef DOXYGEN
        template <class SPolicy>
        static void connect(SocketHandle<SPolicy> handle, Address const & addr,
                            typename IfCommunicationPolicyIs<SPolicy,ConnectedCommunicationPolicy>::type * = 0);
#       else
        template <class SPolicy>
        static void connect(SocketHandle<SPolicy> handle, Address const & addr);
                                        ///< Connect to remote host
                                        /**< This member is only available if the socket handles
                                             communication policy is ConnectedCommunicationPolicy.

                                             \param[in] handle socket handle
                                             \param[in] addr address of remote peer to connect
                                                 to */
#       endif
        static void bind(FileHandle handle, Address const & addr);
                                        ///< Set local socket address
                                        /**< \param[in] handle socket handle
                                             \param[in] addr local socket address */
    };

    /// @}

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "GenericAddressingPolicy.cci"
//#include "GenericAddressingPolicy.ct"
#include "GenericAddressingPolicy.cti"
//#include "GenericAddressingPolicy.mpp"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u ../test"
// comment-column: 40
// End:
