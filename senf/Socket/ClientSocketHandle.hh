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
    \brief ClientSocketHandle public header
 */

#ifndef HH_SENF_Socket_ClientSocketHandle_
#define HH_SENF_Socket_ClientSocketHandle_ 1

// Custom includes
#include <boost/call_traits.hpp>
#include <boost/range/iterator.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/optional.hpp>
#include "SocketHandle.hh"

//#include "ClientSocketHandle.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /// \addtogroup handle_group
    //\{

    template <class SPolicy> class ServerSocketHandle;

    /** \brief Generic SocketHandle with client interface

        This class provides the client side policy interface of the socket
        abstraction. ClientSocketHandle defines the complete policy interface. It does not implement
        any functionality itself however. The following table shows, to which policy members each
        group of ClientSocketHandle members is forwarded. The last column shows, on which other
        policies this member-group depends <em>in the default policy classes</em>. If you define
        your own policy classes, the dependencies are up to you.

        <table class="senf">
        <tr><th>ClientSocketHandle member</th> <th>Policy member</th> <th>Other policies</th></tr>
        <tr><td>read()</td>       <td>ReadPolicy::read (\ref senf::ReadPolicyBase)</td>                  <td></td></tr>
        <tr><td>readfrom()</td>   <td>ReadPolicy::readfrom (\ref senf::ReadPolicyBase)</td>              <td>UnconnectedCommunicationPolicy</td></tr>
        <tr><td>write()</td>      <td>WritePolicy::write (\ref senf::WritePolicyBase)</td>               <td>ConnectedCommunicationPolicy</td></tr>
        <tr><td>writeto()</td>    <td>WritePolicy::writeto (\ref senf::WritePolicyBase)</td>             <td>UnconnectedCommunicationPolicy</td></tr>
        <tr><td>connect()</td>    <td>AddressingPolicy::connect (\ref senf::AddressingPolicyBase)</td>   <td></td></tr>
        <tr><td>bind()</td>       <td>AddressingPolicy::bind (\ref senf::AddressingPolicyBase)</td>      <td></td></tr>
        <tr><td>peer()</td>       <td>AddressingPolicy::peer (\ref senf::AddressingPolicyBase)</td>      <td></td></tr>
        <tr><td>local()</td>      <td>AddressingPolicy::local (\ref senf::AddressingPolicyBase)</td>     <td></td></tr>
        <tr><td></td>             <td></td>                                                              <td></td></tr>
        <tr><td>dequeue()</td>    <td>ReadPolicy::dequeue (\ref senf::ReadPolicyBase)</td>               <td></td></tr>
        <tr><td>release()</td>    <td>ReadPolicy::release (\ref senf::ReadPolicyBase)</td>               <td></td></tr>
        <tr><td>enqueue()</td>    <td>WritePolicy::release (\ref senf::WritePolicyBase)</td>             <td></td></tr>
        <tr><td>send()</td>       <td>WritePolicy::send (\ref senf::WritePolicyBase)</td>                <td></td></tr>
        </table>

        It is important to note, that not all members are always accessible. Which are depends on
        the \c Policy template argument. If any of the policy axis is left unspecified the
        corresponding members will not be callable (you will get a compile time error). Even if
        every policy axis is defined, some members might (and will) not exist if they are
        meaningless for the protocol of the socket. This depends on the exact policy.

        To find out, which members are available, you have to check the documentation of the policy
        classes. You can also find a summary of all members available in the leaf protocol class
        documentation.

        The last set of members is special: it provides an alternative read/write implementation
        available only on special handles supporting the QueueReadPolicy and/or QueueWritePolicy.

        \todo Move all not template-parameter dependent code into a non-template base class

        \idea Give SocketHandle (and therefore ClientSocketHandle and ServerSocketHandle) a \c
        protocol() template member and an additional template arg \c Policies. This arg should be a
        typelist of policy classes which can be accessed. You use protocol<ProtocolClass>() to
        access a protocol class. \c Policies can of course be underspecified or even empty.

        \see \ref policy_group \n
             \ref protocol_group
      */
    template <class SPolicy>
    class ClientSocketHandle
        : public SocketHandle<SPolicy>
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        /// Address type from the addressing policy
        typedef typename SPolicy::AddressingPolicy::Address Address;
        /// 'Best' type for passing address as parameter
        /** Depending on the type of \c Address, this will be either <tt>Address</tt> or <tt>Address
            const &</tt>. See <a
            href="http://www.boost.org/doc/libs/release/libs/utility/call_traits.htm">call_traits documentation in
            the Boost.Utility library.</a>
         */
        typedef typename boost::call_traits<Address>::param_type AddressParam;
        /// Corresponding server socket handle with the same policy
        /** This class will probably only be usable, if the \c CommunicationPolicy is \c
            ConnectedCommunicationPolicy and the \c AddressingPolicy is not \c
            NoAddressingPolicy. */
        typedef ServerSocketHandle<SPolicy> ServerHandle;

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        // default default constructor
        // default copy constructor
        // default copy assignment
        // default destructor

        // here to implement
        ClientSocketHandle();

        // conversion constructors
#       ifndef DOXYGEN
        template <class OtherPolicy>
        ClientSocketHandle(ClientSocketHandle<OtherPolicy> other,
                           typename SocketHandle<SPolicy>::template IsCompatible<OtherPolicy>::type * = 0);
#       else
        ClientSocketHandle(ClientSocketHandle<OtherPolicy> other);
#       endif

#       ifndef DOXYGEN
        template <class OtherPolicy>
        typename SocketHandle<SPolicy>::template IsCompatible<OtherPolicy>::type const &
        operator=(ClientSocketHandle<OtherPolicy> other);
#       else
        template <class OtherPolicy>
        ClientSocketHandle<OtherPolicy> const & operator=(ClientSocketHandle<OtherPolicy> other);
#       endif

        //\}
        //-////////////////////////////////////////////////////////////////////////

        //-////////////////////////////////////////////////////////////////////////
        ///\name Reading and Writing
        //\{

        /** \brief Read data from socket

            If the sockets \c FramingPolicy is \c DatagramFramingPolicy, every read() command will
            return a single datagram. If the sockets FramingPolicy is StreamFraming, the operation
            will return as much data as possible from the socket buffer. However it cannot be
            guaranteed, that the socket buffer will be empty after read() returns.

            \attention If the space available for the data read is limited, the read will return no
            more than that amount of data. For a datagram socket, a full datagram is still dequeued
            from the socket buffer, the remainder of the datagram will be lost.

            There are several variants of read which differ in how they return the read string.

            If the further document doesn't tell something differently, on a blocking socket the
            members will \e always return some data (as long as the socket has not been closed at
            the other end) and will block, if no data is available now. If you do not want to block,
            you \e must make the socket non-blocking (using FileHandle::blocking()).

            \throws senf::SystemException


            This variant will read up to \c limit bytes from the
            socket and return them as a \c std::string object.

            \param[in] limit Maximum number of bytes to read or 0 if unlimited.
            \returns data read

            \implementation The read() family of members will use standard POSIX \c read calls, not
            \c recv.
        */
        std::string  read         (unsigned limit=0);

#       ifndef DOXYGEN
        template <class ForwardWritableRange>
        typename boost::disable_if< boost::is_convertible<ForwardWritableRange,unsigned>,
        typename boost::range_iterator<ForwardWritableRange>::type>::type
        read(ForwardWritableRange const & range);
#       else
        template <class ForwardWritableRange>
        typename boost::range_iterator<ForwardWritableRange>::type
                     read         (ForwardWritableRange const & range);
                                        ///< Read data into range
                                        /**< Read data into the given range. At most
                                             <tt>boost::size(range)</tt> characters are read. The
                                             data read will start at the beginning of the
                                             range. read returns a past-the-end iterator after the
                                             last character read. This iterator will point to
                                             somewhere within the input range.
                                             \param[in,out] range Range to store data in
                                             \returns past-the-end iterator pointer to after the
                                                 last read character
                                             \see \ref read() \n
                                                  <a href="http://www.boost.org/doc/libs/release/libs/range/index.html">Boost.Range</a> */
#       endif
#       ifndef DOXYGEN
        template <class ForwardWritableRange>
        typename boost::disable_if< boost::is_convertible<ForwardWritableRange,unsigned>,
        typename boost::range_iterator<ForwardWritableRange>::type>::type
        read(ForwardWritableRange & range);
#       else
        template <class ForwardWritableRange>
        typename boost::range_iterator<ForwardWritableRange>::type
                     read         (ForwardWritableRange & range);
                                        ///< Read data into range
                                        /**< \see read(ForwardWritableRange const &) \n
                                                  read() \n
                                                  <a href="http://www.boost.org/doc/libs/release/libs/range/index.html">Boost.Range</a>  */
#       endif
        template <class Sequence>
        void         read         (Sequence & container, unsigned limit);
                                        ///< Read data into container
                                        /**< The data read is written into the given container. Old
                                             data in the container will be removed. For this to
                                             work, the container must be a model of 'Sequence' as
                                             defined in the STL documentation
                                             \param[out] container Container to write data to
                                             \param[in] limit Maximum number of characters to read
                                             \see \ref read() */
        char *       read         (char * start, char * end);
                                        ///< Read data into memory area
                                        /**< This variant will read data into the memory area from
                                             \a start to before \a end. This is guaranteed to be the
                                             most efficient version  of read().
                                             \param[in] start address of buffer to store data at
                                             \param[in] end address one past the end of the buffer
                                             \returns pointer past the end of the data read
                                             \see \ref read() */

        /** \brief Read data from unconnected socket returning address

            The readfrom() group of member behaves like \ref read() but should only be available, if
            the sockets \c CommunicationPolicy is \c UnconnectedCommunicationPolicy and the \c
            AddressingPolicy is not \c NoAddressingPolicy. readfrom() will in addition to the data
            return the address of the sender.

            \throws senf::SystemException


            This variant will return the data read and the address as a std::pair.

            \returns \c std::pair of data read (a string) and the peers address

            \implementation The readfrom() family of members will use \c recvfrom from the BSD
            socket API.
         */
        std::pair<std::string, Address>
                     readfrom     (unsigned limit=0);
        template <class ForwardWritableRange>
        typename boost::range_iterator<ForwardWritableRange const>::type
                     readfrom     (ForwardWritableRange const & range, Address & from);
                                        ///< Read data into range
                                        /**< Read data into the given range. At most
                                             <tt>boost::size(range)</tt> characters are read. The
                                             data read will start at the beginning of the
                                             range. read returns a past-the-end iterator after the
                                             last character read. This iterator will point to
                                             somewhere within the input range.
                                             \param[in,out] range Range to store data in
                                             \param[out] from peers address from which the data was
                                                 received
                                             \returns past-the-end iterator pointer to after the
                                                 last read character
                                             \see \ref readfrom() \n
                                                  <a href="http://www.boost.org/doc/libs/release/libs/range/index.html">Boost.Range</a>  */
        template <class ForwardWritableRange>
        typename boost::range_iterator<ForwardWritableRange>::type
                     readfrom     (ForwardWritableRange & range, Address & from);
                                        ///< Read data into range
                                        /**< \see readfrom(ForwardWritableRange const&,Address&) \n
                                                  readfrom()  \n
                                                  <a href="http://www.boost.org/doc/libs/release/libs/range/index.html">Boost.Range</a> */
        template <class Sequence>
        void         readfrom     (Sequence & container, Address & from, unsigned limit);
                                        ///< Read data into container
                                        /**< The data read is written into the given container. Old
                                             data in the container will be removed. For this to
                                             work, the container must be a model of 'Sequence' as
                                             defined in the STL documentation
                                             \param[out] container Container to write data to
                                             \param[in] limit Maximum number of characters to read
                                             \param[out] from peers address from which the data was
                                                 received
                                             \see \ref readfrom() */
        char *       readfrom     (char * start, char * end, Address & from);
                                        ///< Read data into memory buffer
                                        /**< This variant will read data into the memory area from
                                             \a start to before \a end. This is guaranteed to be the
                                             most efficient version  of readfrom().
                                             \param[in] start address of buffer to store data at
                                             \param[in] end address one past the end of the buffer
                                             \param[out] from peers address from which the data was
                                                 received
                                             \returns pointer past the end of the data read
                                             \see \ref read() */


        /** \brief Write data to socket

            The write() family of members will write out the data to the socket.  If the sockets \c
            FramingPolicy is \c DatagramFramingPolicy, every write() call will result in one
            datagram.

            A single write call might depending on the circumstances write only part of the data.

            There are two variants of this member

            \throws senf::SystemException


            This variant will write out the range \c data.

            \param[in] range Data to write
            \returns past-the-end iterator after last element written
            \implementation The write() family of members will use POSIX \c write calls, not \c
                send.
         */
        template <class ForwardReadableRange>
        typename boost::range_const_iterator<ForwardReadableRange const>::type
                     write        (ForwardReadableRange const & range);
        char const * write        (char const * start, char const * end);
                                        ///< Write data to socket from memory buffer
                                        /**< \param[in] start beginning of area to write
                                             \param[in] end past-the-end pointer to area to write
                                             \returns past-the-end pointer after last byte written
                                             \see \ref write() \n
                                                  <a href="http://www.boost.org/doc/libs/release/libs/range/index.html">Boost.Range</a>  */

        /** \brief Write data to unconnected socket

            This member behaves like write() but should only be available, if the sockets \c
            CommunicationPolicy is \c UnconnectedCommunicationPolicy and the \c AddressingPolicy is
            not \c NoAddressingPolicy. The writeto() family of members takes the target address as
            an additional argument.

            There are two variants of this member.

            \throw senf::SystemException


            This variant will send the range \c range to peer \c addr.

            \param[in] addr Address of peer to send data to
            \param[in] range data to send
            \returns Number of bytes written
         */
        template <class ForwardReadableRange>
        typename boost::range_const_iterator<ForwardReadableRange const>::type
                     writeto      (AddressParam addr, ForwardReadableRange const & range);
        char const * writeto      (AddressParam addr, char const * start, char const * end);
                                        ///< Write data from memory buffer to unconnected socket
                                        /**< \param[in] addr Address of peer to send data to
                                             \param[in] start address of buffer to write
                                             \param[in] end past-the-end pointer after data to write
                                             \returns past-the-end iterator after last byte written
                                             \see \ref writeto() \n
                                                  <a href="http://www.boost.org/doc/libs/release/libs/range/index.html">Boost.Range</a>  */

        //\}

        //-////////////////////////////////////////////////////////////////////////
        ///\name Queue based Reading and Writing

        /** \brief Read data from packet queue

            Some protocols support a more efficient queue based read/write protocol. If the handle
            supports this protocol, \ref dequeue() and \ref release() are used to read packets from
            that queue.

            The API works as follows:

            \li calling \ref dequeue() will return a buffer object pointing to the packet data in
                the read queue.
            \li to release the memory returned by dequeue(), call \ref release(). This call will
                return \e all unreleased frames to the kernel.

            The API has some special features:

            \li the API is always non-blocking
            \li you must release() all data back to the kernel before re-entering the
                scheduler. Otherwise, the file handle will be signaled again immediately.
            \li even if the handle is marked readable \ref dequeue() might return \c boost::none.

            \returns iterator range to the data frame or \c boost::none, if no data is available

            \implementation the reason, \ref dequeue() might return \c boost::none even if readable
                is, that on a packet socket handle outgoing data is sent to the socket but is
                filtered in \ref dequeue()
         */
        boost::optional<typename SPolicy::ReadPolicy::Buffer> dequeue();

        void release();                 ///< Release all queue frames to the kernel
                                        /**< All the frames dequeued since the last call to
                                             release() will be returned to kernel space. */

        /** \brief Write data to packet queue

            Some protocols support a more efficient queue based read/write protocol. If the handle
            supports this protocol, \ref enqueue() and \ref release() are used to
            write packets to that queue.

            The API works as follows:

            \li calling \ref enqueue() will return an buffer object pointing to a new queue
                entry.
            \li now write the data frame to that area (e.g. memcpy the packet there or construct a
                new packet in the memory area using the external packet memory management support)
            \li you mast call \c resize() on the buffer object to set the size of the packet
                to be sent
            \li call \ref send() to send all packets allocated from the send queue since the last
                call call to \c send()

            The API has some special features:

            \li you may call \ref enqueue() several times before calling \ref send()
            \li a call to \ref send() will send out all packets allocated since the last call to
                \ref send()
            \li it is possible for the call to \ref enqueue() to fail if no space is available in
                the send queue

            \returns iterator range to the write queue buffer or \c boost::none, if there is no room
                in the write queue.

            \warning You must ensure to call \ref \a bufffer.\c resize() on all packets you \ref
                enqueue() before calling \ref send()
         */
        boost::optional<typename SPolicy::WritePolicy::Buffer> enqueue();

        bool send();                       ///< Send all data in the write queue
                                           /**< This call will send out all frames allocated since the
                                              last call to \ref send(). */

        //\}
        //-////////////////////////////////////////////////////////////////////////
        ///\name Addressing
        //\{

        /** \brief Connect to remote peer

            This member will establish a connection for addressable connection-oriented protocols
            (that is, the CommunicationPolicy is ConnectedCommunicationPolicy and the
            AddressingPolicy is not NoAddressingPolicy).

            \param[in] addr Address to connect to

            \throws senf::SystemException
         */
        void         connect      (AddressParam addr) const;

        /** \brief Set local address

            For addressable protocols (AddressingPolicy is not NoAddressingPolicy), bind() will set
            the local address of the socket.

            \param[in] addr Local socket address to assign

            \throws senf::SystemException
         */
        void         bind         (AddressParam addr) const;

        /** \brief Query remote address

            This member will return the address of the communication partner in addressable
            connection-oriented protocols (that is, the CommunicationPolicy is
            ConnectedCommunicationPolicy and the AddressingPolicy is not NoAddressingPolicy).

            There are two Variants of this member, one will return the address by value, the other
            takes a reference argument to elide the copy operation.

            \throws senf::SystemException
         */
        Address      peer         () const;
        void         peer         (Address & addr) const;
                                        ///< Query remote address
                                        /**< \see \ref peer() */

        /** \brief Query local address

            This member will return the address of the local socket in addressable protocols
            (AddressingPolicy is not NoAddressingPolicy).

            There are two Variants of this member, one will return the address by value, the other
            takes a reference argument to elide the copy operation.

            \throws senf::SystemException
         */
        Address      local        () const;
        void         local        (Address & addr) const;
                                        ///< Query local address
                                        /**< \see \ref local() */

        //\}

        static ClientSocketHandle cast_static(FileHandle handle);
        static ClientSocketHandle cast_dynamic(FileHandle handle);

        // we need to override both since SocketHandle is *not* polymorphic
        void state(SocketStateMap & map, unsigned lod=0);
        std::string dumpState(unsigned lod=0);

        unsigned available();

    protected:
        ClientSocketHandle(FileHandle other, bool isChecked);
        explicit ClientSocketHandle(std::unique_ptr<SocketBody> body);

    private:
        friend class senf::ServerSocketHandle<SPolicy>;
    };

    //\}
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "ClientSocketHandle.cci"
#include "ClientSocketHandle.ct"
#include "ClientSocketHandle.cti"
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
