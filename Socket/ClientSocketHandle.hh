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
    \brief ClientSocketHandle public header
 */

#ifndef HH_ClientSocketHandle_
#define HH_ClientSocketHandle_ 1

// Custom includes
#include <boost/call_traits.hpp>
#include "SocketHandle.hh"

//#include "ClientSocketHandle.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
    
    /// \addtogroup handle_group
    /// @{
    
    template <class Policy> class ServerSocketHandle;

    /** \brief Generic SocketHandle with client interface
	
	This class provides the client side policy interface of the socket
	abstraction. ClientSocketHandle defines the complete policy interface. It does not implement
	any functionality itself however. All calls are forward to the following policy classes:

	<table class="senf">
	<tr><th>ClientSocketHandle member</th> <th>Policy member</th></tr>
	<tr><td>read()</td>       <td>ReadPolicy::read (\ref senf::ReadPolicyBase)</td></tr>
	<tr><td>readfrom()</td>   <td>ReadPolicy::readfrom (\ref senf::ReadPolicyBase)</td></tr>
	<tr><td>write()</td>      <td>WritePolicy::write (\ref senf::WritePolicyBase)</td></tr>
	<tr><td>writeto()</td>    <td>WritePolicy::writeto (\ref senf::WritePolicyBase)</td></tr>
	<tr><td>connect()</td>    <td>AddressingPolicy::connect (\ref senf::AddressingPolicyBase)</td></tr>
	<tr><td>bind()</td>       <td>AddressingPolicy::bind (\ref senf::AddressingPolicyBase)</td></tr>
	<tr><td>peer()</td>       <td>AddressingPolicy::peer (\ref senf::AddressingPolicyBase)</td></tr>
	<tr><td>local()</td>      <td>AddressingPolicy::local (\ref senf::AddressingPolicyBase)</td></tr>
	<tr><td>rcvbuf()</td>     <td>BufferingPolicy::sndbuf (\ref senf::BufferingPolicyBase)</td></tr>
	<tr><td>sndbuf()</td>     <td>BufferingPolicy::rcvbuf (\ref senf::BufferingPolicyBase)</td></tr>
	</table>

	It is important to note, that not all members are always accessible. Which are depends on
	the \c Policy template argument. If any of the policy axis is left unspecified the
	corresponding members will not be callable (you will get a compile time error). Even if
	every policy axis is defined, some members might (and will) not exist if they are
	meaningless for the protocol of the socket. This depends on the exact policy.

	To find out, which members are available, you have to check the documentation of the policy
	classes. You can also find a summary of all members available in the leaf protocol class
	documentation.

	\todo Move all not template-parameter dependent code into a non-template base class

	\idea Give SocketHandle (and therefore ClientSocketHandle and ServerSocketHandle) a \c
	protocol() template member and an additional template arg \c Policies. This arg should be a
	typelist of Poclicy classes which can be accessed. You use protocol<ProtocolClass>() to
	access a protocol class. \c Policies can of course be underspecified or even empty.

	\idea add more flexible read/write members for a) boost::arrays and arrays of other types b)
	std::vector (which uses contiguous memory ..) c) other random-access containers (we should
	use some configurable trait class to identify containers with contiguous storage). Probably
	we should just use a generic Boost.Range interface. Here we again come to the point: make
	all except the most basic members be non-member algorithms ? this would make the
	configuration of such extenden members more flexible.

	\see \ref policy_group \n
             \ref protocol_group
      */
    template <class Policy>
    class ClientSocketHandle
        : public SocketHandle<Policy>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

	/// Address type from the addressing policy
        typedef typename Policy::AddressingPolicy::Address Address;
	/// 'Best' type for passing address as parameter
	/** Depending on the type of \c Address, this will be either <tt>Address</tt> or <tt>Address
	    const &</tt>. See <a href="http://www.boost.org/libs/utility/call_traits.htm"
	    class="ext">call_traits documentation in the Boost.Utility library\endlink.</a>
	 */
        typedef typename boost::call_traits<Address>::param_type AddressParam;
	/// Corresponding server socket handle with the same policy
	/** This class will probably only be usable, if the \c CommunicationPolicy is \c
	    ConnectedCommunicationPolicy and the \c AddressingPolicy is not \c
	    NoAddressingPolicy. */
        typedef ServerSocketHandle<Policy> ServerSocketHandle;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // no default constructor
        // default copy constructor
        // default copy assignment
        // default destructor

        // conversion constructors
        template <class OtherPolicy>
        ClientSocketHandle(ClientSocketHandle<OtherPolicy> other,
                           typename SocketHandle<Policy>::template IsCompatible<OtherPolicy>::type * = 0);

        template <class OtherPolicy>
        typename SocketHandle<Policy>::template IsCompatible<OtherPolicy>::type const & 
        operator=(ClientSocketHandle<OtherPolicy> other);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////////////////////////////////////
        ///\name Reading and Writing
        ///@{

	/** \brief Read data from socket

	    If the sockets \c FramingPolicy is \c DatagramFramingPolicy, every read() command will
	    return a single datagram. If the sockets FramingPolicy is StreamFraming, the operation will
	    return as much data as possible from the socket buffer. However it cannot be guaranteed,
	    that the socket buffer will be empty after read() returns.

	    \attention If the space available for the data read is limited, the read will return no
	    more than that amount of data. For a datagram socket, a full datagram is still dequed
	    from the socket buffer, the remainder of the datagram will be lost.

	    There are three variants of read which differ in how they return the read string. 

	    \throws senf::SystemException


	    This variant will read up to \c limit bytes from the
	    socket and return them as a \c std::string object.

	    On a blocking socket, this member will \e always return some data (as long as the socket
	    has not been closed at the other end) and will block, if no data is available now. If
	    you do not want to block, you \e must make the socket non-blocking (using
	    FileHandle::blocking()).

	    \param[in] limit Maximum number of bytes to read or 0 if unlimited.
	    \returns data read
	    
	    \implementation The read() family of members will use standard POSIX \c read calls, not
	    \c recv.
	*/
        std::string  read         (unsigned limit=0);
        void         read         (std::string & buffer, unsigned limit=0);
                                        ///< Read data into string buffer
                                        /**< On a blocking socket, this member will \e always return
					   some data (as long as the socket has not been closed at
					   the other end) and will block, if no data is available
					   now. If you do not want to block, you \e must make the
					   socket non-blocking (using FileHandle::blocking()).
					   \param[out] buffer data read
					   \param[in] limit Maximum number of buytes to read or 0
					   if unlimited 
					   \see \ref read() */
        unsigned     read         (char * buffer, unsigned size);
                                        ///< Read data into memory area
                                        /**< This variant will read data into the memory area at \c
					   buffer of size \c size. This is the most performant
					   version of read().
					   \param[in] buffer address of buffer to store data at
					   \param[in] size size of memory buffer
					   \returns Number of bytes read 
					   \see \ref read() */

	/** \brief Read data from unconnected socket returning address

	    This member behaves like read() but should only be available, if the sockets \c
	    CommunicationPolicy is \c UnconnectedCommunicationPolicy and the \c AddressingPolicy is
	    not \c NoAddressingPolicy. The readfrom() family will in addition to the data return the
	    address of the sender.

	    \throws senf::SystemException

	    This variant will return the data read and the address as a std::pair.

	    \returns \c std::pair of data read (a string) and the peers address

	    \todo Add \c limit argument

	    \implementation The readfrom() family of members will use \c recvfrom from the BSD
	    socket API.
	 */
	std::pair<std::string, Address> 
                     readfrom     ();
        void         readfrom     (std::string & buffer, Address & from);
                                        ///< Read data into string buffer
                                        /**< This variant will return the result in the locations
					   passed in
					   \param[out] buffer data read
					   \param[out] from peer address
					   \see \ref readfrom() */
        unsigned     readfrom     (char * buffer, unsigned size, Address & from);
                                        ///< Read data into memory byffer
                                        /**< This variant will read data into the memory area at \c
					   buffer of size \c size. This is the most performant
					   version of readfrom().
					   \param[in] buffer address of buffer to store data at
					   \param[in] size size of bnuffer
					   \param[out] from peer address
					   \returns Number of bytes read
					   \see \ref readfrom() */
	

	/** \brief Write data to socket

	    The write() family of members will write out the data to the socket.  If the sockets \c
	    FramingPolicy is \c DatagramFramingPolicy, every write() call will result in one
	    datagram.

	    A single write call might depending on the circumstances write only part of the data.

	    There are two variants of thie member
	    
	    \throws senf::SystemException

	    
	    This variant will write out the string \c data.
	    
	    \param[in] data Data to write
	    \returns number of bytes written
	    \todo Make this member write the complete string if the socket is blocking
	    \implementation The write() family of members will use POSIX \c write calls, not \c
		send.
	 */
        unsigned     write        (std::string const & data);
        unsigned     write        (char const * buffer, unsigned size);
                                        ///< Write data to socket from memory buffer
                                        /**< \param[in] buffer address of buffer to write
					   \param[in] size amount of data to write
					   \returns Number of bytes written
					   \see \ref write() */

	/** \brief Write data to unconnected socket

	    This member behaves like write() but should only be available, if the sockets \c
	    CommunicationPolicy is \c UnconnectedCommunicationPolicy and the \c AddressingPolicy is
	    not \c NoAddressingPolicy. The writeto() family of members takes the target address as
	    an additional argument.

	    There are two variants of this member.

	    \throw senf::SystemException

	    
	    This variant will send the string \c data to the peer \c addr.

	    \param[in] addr Address of peer to send data to
	    \param[in] data data to send
	    \returns Number of bytes written
	 */
	unsigned     writeto      (AddressParam addr, std::string const & data);
	unsigned     writeto      (AddressParam addr, char const * buffer, unsigned size);
                                        ///< Write data from memory buffer to unconnected socket
                                        /**< \param[in] addr Address o fpeer to send data to
					   \param[in] buffer address of buffer to write
					   \param[in] size amount of data to write
					   \returns Number of bytes written 
					   \see \ref writeto() */

	///////////////////////////////////////////////////////////////////////////
	///\name Addressing
	///@{

	/** \brief Connect to remote peer

	    This member will establish a connection for addressable connection-oriented protocols
	    (that is, the CommunicationPolicy is ConnectedCommunicationPolicy and the
	    AddressingPolicy is not NoAddressingPolicy). 

	    \param[in] addr Address to connect to

	    \throws senf::SystemException
	 */
	void         connect      (AddressParam addr);

	/** \brief Set local address
	    
	    For addressable protocols (AddressingPolicy is not NoAddressingPolicy), bind() will set
	    the local address of the socket.

	    \parm[in] addr Local socket address to asign

	    \throws senf::SystemException
	 */
	void         bind         (AddressParam addr);

	/** \brief Query remote address

	    This member will return the address of the communication partner in addressable
	    connection-oriented protocols (that is, the CommunicationPolicy is
	    ConnectedCommunicationPolicy and the AddressingPolicy is not NoAddressingPolicy).

	    There are two Variants of this member, one will return the address by value, the other
	    takes a reference argument to elide the copy operation.

	    \throws senf::SystemException
	 */
	Address      peer         ();
	void         peer         (Address & addr);
                                        ///< Query remote address
                                        /**< \see \ref peer() */

	/** \brief Query local address

	    This member will return the address of the local socket in addressable protocols
	    (AddressingPolicy is not NoAddressingPolicy).

	    There are two Variants of this member, one will return the address by value, the other
	    takes a reference argument to elide the copy operation.

	    \throws senf::SystemException
	 */
	Address      local        ();
	void         local        (Address & addr);
                                        ///< Query local address
                                        /**< \see \ref local() */

	///@}

	///////////////////////////////////////////////////////////////////////////
	///\name Buffering
	///@{

	unsigned     rcvbuf      ();    ///< Check size of receive buffer
                                        /**< \returns size of receive buffer in bytes */
	void         rcvbuf      (unsigned size);
                                        ///< Set size of receive buffer
                                        /**< \param[in] size size of receive buffer in bytes */

	unsigned     sndbuf      ();    ///< Check size of send buffer
                                        /**< \returns size of send buffer in bytes */
	void         sndbuf      (unsigned size);
                                        ///< Set size of send buffer
                                        /**< \param[in] size size of send buffer in bytes */

	///@}

	static ClientSocketHandle cast_static(FileHandle handle);
	static ClientSocketHandle cast_dynamic(FileHandle handle);

	// we need to override both since SocketHandle is *not* polymorphic
	void state(SocketStateMap & map, unsigned lod=0);
	std::string dumpState(unsigned lod=0);

    protected:
	ClientSocketHandle(FileHandle other, bool isChecked);
	explicit ClientSocketHandle(std::auto_ptr<SocketProtocol> protocol,
				    int fd = -1);

    private:
	unsigned available();

	friend class senf::ServerSocketHandle<Policy>;
    };

    /// @}
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "ClientSocketHandle.cci"
#include "ClientSocketHandle.ct"
#include "ClientSocketHandle.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "senf"
// fill-column: 100
// End:
