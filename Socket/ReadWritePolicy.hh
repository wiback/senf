// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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
    \brief ReadPolicy and WritePolicy public header

    \todo ReadWritePolicy.test.cc
 */

#ifndef HH_ReadWritePolicy_
#define HH_ReadWritePolicy_ 1

// Custom includes
#include "SocketPolicy.hh"
#include "ClientSocketHandle.hh"
#include "CommunicationPolicy.hh"

//#include "ReadWritePolicy.mpp"
///////////////////////////////hh.p////////////////////////////////////////


struct sockaddr;

namespace senf {

    /// \addtogroup policy_impl_group
    /// @{

    /** \brief ReadPolicy for readable sockets

        This policy provides support for readable sockets via the standard UNIX read/recvfrom system
        calls. The concrete semantics of the read calls depend on the framing policy of the socket.
     */
    struct ReadablePolicy : public ReadPolicyBase
    {
        static unsigned read(FileHandle handle, char * buffer, unsigned size);
                                        ///< read data from socket
                                        /**< \param[in] handle socket handle to read from
                                             \param[in] buffer address of buffer to write data to
                                             \param[in] size size of buffer
                                             \returns number of bytes read */
#       ifndef DOXYGEN
        template <class Policy>
        static unsigned readfrom(ClientSocketHandle<Policy> handle, char * buffer, unsigned size,
                                 typename Policy::AddressingPolicy::Address & address,
                                 typename IfCommunicationPolicyIs<
                                     Policy,UnconnectedCommunicationPolicy>::type * = 0);
#       else
        template <class Policy>
        static unsigned readfrom(ClientSocketHandle<Policy> handle, char * buffer, unsigned size,
                                 typename Policy::AddressingPolicy::Address & address);
                                        ///< read data from socket returning peer address
                                        /**< \param[in] handle socket handle to read from
                                             \param[in] buffer address of buffer to write data to
                                             \param[in] size size of buffer
                                             \param[out] address peer address
                                             \returns number of bytes read */
#       endif

    private:
        static unsigned do_readfrom(FileHandle handle, char * buffer, unsigned size,
                                    struct ::sockaddr * addr, socklen_t len);
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
        template <class Policy>
        static unsigned write(ClientSocketHandle<Policy> handle, char const * buffer, unsigned size,
                              typename IfCommunicationPolicyIs<
                                  Policy,ConnectedCommunicationPolicy>::type * = 0);
#       else
        template <class Policy>
        static unsigned write(ClientSocketHandle<Policy> handle, char const * buffer, 
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
        template <class Policy>
        static unsigned writeto(ClientSocketHandle<Policy> handle,
                                typename boost::call_traits<
                                    typename Policy::AddressingPolicy::Address>::param_type addr,
                                char const * buffer, unsigned size,
                                typename IfCommunicationPolicyIs<
                                    Policy,UnconnectedCommunicationPolicy>::type * = 0);
#       else
        template <class Policy>
        static unsigned writeto(ClientSocketHandle<Policy> handle,
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
        static unsigned do_write(FileHandle handle, char const * buffer, unsigned size);
        static unsigned do_writeto(FileHandle handle, char const * buffer, unsigned size,
                                   struct sockaddr const * addr, socklen_t len);
    };

    /** \brief WritePolicy for unwriteable sockets

        This is different from UndefinedWritePolicy (which is the same as WritePolicyBase). This
        policy class defines the socket writeability -- it explicitly states, that the socket does
        not support writing.
     */
    struct NotWriteablePolicy : public WritePolicyBase
    {};

    /// @}

}


///////////////////////////////hh.e////////////////////////////////////////
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
