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
    \brief ServerSocketHandle public header
 */

#ifndef HH_ServerSocketHandle_
#define HH_ServerSocketHandle_ 1

// Custom includes
#include <boost/static_assert.hpp>
#include <boost/call_traits.hpp>
#include "SocketHandle.hh"
#include "CommunicationPolicy.hh"
#include "AddressingPolicy.hh"

//#include "ServerSocketHandle.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup handle_group
    /// @{

    template <class Policy> class ClientSocketHandle;

    /** \brief Generic SocketHandle with server interface

	This class provides the server side policy interface of the socket abstraction.
	ServerSocketHandle defines the complete policy interface. It does not implement any
	functionality itself however. All calls are forwarded to the following policy classes:

	<table class="senf">
	<tr><td>senf::ServerSocketHandle::bind</td>       <td>AddressingPolicy::bind (\ref senf::AddressingPolicyBase)</td></tr>
	<tr><td>senf::ServerSocketHandle::listen</td>     <td>CommunicationPolicy::listen (\ref senf::CommunicationPolicyBase)</td></tr>
	<tr><td>senf::ServerSocketHandle::local</td>      <td>AddressingPolicy::local (\ref senf::AddressingPolicyBase)</td></tr>
	<tr><td>senf::ServerSocketHandle::accept</td>     <td>CommunicationPolicy::accept (\ref senf::CommunicationPolicyBase)</td></tr>
	<tr><td>senf::ServerSocketHandle::acceptfrom</td> <td>CommunicationPolicy::accept (\ref senf::CommunicationPolicyBase)</td></tr>
	</table>

	A ServerSocketHandle is only meaningful for connection oriented addressable protocols
	(CommunicationPolicy is ConnectedCommunicationPolicy and AddressingPolicy is not
	NoAddressingPolicy).

	It is important to note, that not all members are always accessible. Which are depends on
	the \c Policy template argument. If any of the policy axis is left unspecified the
	corresponding members will not be callable (you will get a compile time error). Even if
	every policy axis is defined, some members might (and will) not exist if they are
	meaningless for the protocol of the socket. This depends on the exact policy.

	To find out, which members are available, you have to check the documentation of the policy
	classes. You can also find a summary of all members available in the leaf protocol class
	documentation.
      */
    template <class Policy>
    class ServerSocketHandle
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
	/// Corresponding client socket handle with the same policy
        typedef ClientSocketHandle<Policy> ClientSocketHandle;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // no default constructor
        // default copy constructor
        // default copy assignment
        // default destructor

        // conversion constructors
        template <class OtherPolicy>
        ServerSocketHandle(ServerSocketHandle<OtherPolicy> other,
                           typename SocketHandle<Policy>::template IsCompatible<OtherPolicy>::type * = 0);

        template <class OtherPolicy>
        typename SocketHandle<Policy>::template IsCompatible<OtherPolicy>::type const & 
        operator=(ServerSocketHandle<OtherPolicy> other);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////////////////////////////////////
        ///\name Server socket interface
        ///@{

	/** \brief Set local address
	    
	    For addressable protocols (AddressingPolicy is not NoAddressingPolicy), bind() will set
	    the local address of the socket.

	    \parm[in] addr Local socket address to asign

	    \throws senf::SystemException
	 */
        void         bind         (AddressParam addr);

	/** \brief Allow clients to connect to this server socket
	   
	    \todo This is very protocol specific, I don't want it in the policy
		interface. Especially the backlog argument seems quite protocol specific to
		me. However, we cannot listen() before we bind() so listen() cannot reside in the
		constructor. We need to find a good solution here.

	    \throws senf::SystemException
	 */
	// Possible solution: Make listen() an abstract method of the protocol interface, make the
	// backlog parameter into a member living in the body or protocol class and set it using
	// some accessor. Hmm ... this all seems somehow futile ...
        void         listen       (unsigned backlog=0);

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
	
	/** \brief Accept new connection

	    If the handle is non-blocking, accept will NOT block. If no connection
	    is available to be returned, accept will return a ClientSocketHandle
	    which is not valid()

	    \throws senf::SystemException

	    This variant ...

	    \returns handle of new client connection
	 */
        ClientSocketHandle 
                     accept       ();
        std::pair<ClientSocketHandle, Address>
	             acceptfrom   ();   ///< Accept new connection
                                        /**< This variant will additionally return the remote
					   address of the client
					   \returns \c std::pair with client handle and client
					   address. 
					   \see \ref accept() */
        ClientSocketHandle
                     acceptfrom   (Address & addr);
                                        ///< Accept new connection
                                        /**< This variant will additionally return the remote
					   address of the client
					   \params[out] client address
					   \returns handle of new client connection
					   \see \ref accept() */
        
        ///@}

        static ServerSocketHandle cast_static(FileHandle handle);
        static ServerSocketHandle cast_dynamic(FileHandle handle);

        // we need to override both since SocketHandle is *not* polymorphic
        void state(SocketStateMap & map, unsigned lod=0);
        std::string dumpState(unsigned lod=0);

    protected:
        ServerSocketHandle(FileHandle other, bool isChecked);
        explicit ServerSocketHandle(std::auto_ptr<SocketProtocol> protocol);

    private:
        
    };

    /// @}
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "ServerSocketHandle.cci"
//#include "ServerSocketHandle.ct"
#include "ServerSocketHandle.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "senf"
// fill-column: 100
// End:
