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
    \brief ReadPolicy and WritePolicy public header

    \todo ReadWritePolicy.test.cc
 */

#ifndef HH_SENF_Socket_ReadWritePolicy_
#define HH_SENF_Socket_ReadWritePolicy_ 1

// Custom includes
#include "SocketPolicy.hh"
#include "ClientSocketHandle.hh"
#include "CommunicationPolicy.hh"

//#include "ReadWritePolicy.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////


struct sockaddr;

namespace senf {

    /// \addtogroup policy_impl_group
    //\{

    /** \brief ReadPolicy for readable sockets

        This policy provides support for readable sockets via the standard UNIX read/recvfrom system
        calls. The concrete semantics of the read calls depend on the framing policy of the socket.
     */
    struct ReadablePolicy : public ReadPolicyBase
    {
        static unsigned read(FileHandle & handle, char * buffer, unsigned size);
                                        ///< read data from socket
                                        /**< \param[in] handle socket handle to read from
                                             \param[in] buffer address of buffer to write data to
                                             \param[in] size size of buffer
                                             \returns number of bytes read */
#       ifndef DOXYGEN
        template <class SPolicy>
        static unsigned readfrom(ClientSocketHandle<SPolicy> & handle, char * buffer, unsigned size,
                                 typename SPolicy::AddressingPolicy::Address & address,
                                 typename IfCommunicationPolicyIs<
                                     SPolicy,UnconnectedCommunicationPolicy>::type * = 0);
#       else
        template <class SPolicy>
        static unsigned readfrom(ClientSocketHandle<SPolicy> & handle, char * buffer, unsigned size,
                                 typename Policy::AddressingPolicy::Address & address);
                                        ///< read data from socket returning peer address
                                        /**< \param[in] handle socket handle to read from
                                             \param[in] buffer address of buffer to writedata to
                                             \param[in] size size of buffer
                                             \param[out] address peer address
                                             \returns number of bytes read */
#       endif

    private:
        static unsigned do_readfrom(FileHandle & handle, char * buffer, unsigned size,
                                    struct ::sockaddr * addr, socklen_t * len);
    };

    /** \brief ReadPolicy for unreadable sockets

        This is different from UndefinedReadPolicy (which is the same as ReadPolicyBase). This
        policy class defines the socket readability -- it explicitly states, that the socket does
        not support reading.
     */
    struct NotReadablePolicy : public ReadPolicyBase
    {};

    /** \brief WritePolicy for writeable sockets

        This policy provides support for writable sockets via the standard UNIX write/sendto system
        calls. The concrete semantics of the write calls depend on the framing policy of the socket.
     */
    struct WriteablePolicy : public WritePolicyBase
    {
#       ifndef DOXYGEN
        template <class SPolicy>
        static unsigned write(ClientSocketHandle<SPolicy> & handle, char const * buffer, unsigned size,
                              typename IfCommunicationPolicyIs<
                                  SPolicy,ConnectedCommunicationPolicy>::type * = 0);
#       else
        template <class SPolicy>
        static unsigned write(ClientSocketHandle<SPolicy> & handle, char const * buffer,
                              unsigned size);
                                        ///< write data to socket
                                        /**< This member is only enabled if the socket uses
                                             connected communication. Otherwise the communication
                                             partner must be specified explicitly using the sendto
                                             call

                                             \param[in] handle socket handle to write data to
                                             \param[in] buffer address of buffer to send
                                             \param[in] size number of bytes to write
                                             \returns number of bytes written */
#       endif
#       ifndef DOXYGEN
        template <class SPolicy>
        static unsigned writeto(ClientSocketHandle<SPolicy> & handle,
                                typename boost::call_traits<
                                    typename SPolicy::AddressingPolicy::Address>::param_type addr,
                                char const * buffer, unsigned size,
                                typename IfCommunicationPolicyIs<
                                    SPolicy,UnconnectedCommunicationPolicy>::type * = 0);
#       else
        template <class SPolicy>
        static unsigned writeto(ClientSocketHandle<SPolicy> & handle,
                                typename Policy::AddressingPolicy::Address const & addr,
                                char const * buffer, unsigned size);
                                        ///< write data to socket sending to given peer
                                        /**< This member is only enabled if the socket uses
                                             unconnected communication. Otherwise no target may be
                                             specified since it is implied in the connection.

                                             \param[in] handle socket handle to write data to
                                             \param[in] buffer address of buffer to send
                                             \param[in] size number of bytes to write
                                             \param[in] addr peer to send data to
                                             \returns number of bytes written
                                          */
#       endif

    private:
        static unsigned do_write(FileHandle & handle, char const * buffer, unsigned size);
        static unsigned do_writeto(FileHandle & handle, char const * buffer, unsigned size,
                                   struct sockaddr const * addr, socklen_t len);
    };

    /** \brief WritePolicy for unwriteable sockets

        This is different from UndefinedWritePolicy (which is the same as WritePolicyBase). This
        policy class defines the socket writeability -- it explicitly states, that the socket does
        not support writing.
     */
    struct NotWriteablePolicy : public WritePolicyBase
    {};

    //\}

}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "ReadWritePolicy.cci"
//#include "ReadWritePolicy.ct"
#include "ReadWritePolicy.cti"
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
