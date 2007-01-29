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
    \brief senf::ClientSocketHandle public header
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
	
	This class provides the client side policy interface of the
	socket abstraction. ClientSocketHandle defines the complete
	policy interface. It does not implement any functionality
	itself however. All calls are forward to the following policy
	classes:

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

	It is important to note, that not all members are always
	accessible. Which are depends on the \c Policy template
	argument. If any of the policy axis is left unspecified the
	corresponding members will not be callable (you will get a
	compile time error). Even if every policy axis is defined,
	some members might (and will) not exist depending on the exact
	policy. To find out, which members are available, you have to
	check the documentation of the policy classes. You can also
	find a summary of all active members in the leaf protocol
	class documentation.

	\todo Move all not template-parameter dependent code into a
	non-template base class

	\idea Give SocketHandle (and therefore ClientSocketHandle and
	ServerSocketHandle) a \c protocol() template member and an
	additional template arg \c Policies. This arg should be a
	typelist of Poclicy classes which can be accessed. You use
	protocol<ProtocolClass>() to access a protocol class. \c
	Policies can of course be underspecified or even empty.

	\idea add more flexible read/write members for a)
	boost::arrays and arrays of other types b) std::vector (which
	uses contiguous memory ..) c) other random-access containers
	(we should use some configurable trait class to identify
	containers with contiguous storage). Probably we should just
	use a generic Boost.Range interface. Here we again come to the
	point: make all except the most basic members be non-member
	algorithms ? this would make the configuration of such
	extenden members more flexible.

	\see \ref policy_group
             \ref protocol_group
      */
    template <class Policy>
    class ClientSocketHandle
        : public SocketHandle<Policy>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef typename Policy::AddressingPolicy::Address Address;
        typedef typename boost::call_traits<Address>::param_type AddressParam;
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

        // read from socket (connected or unconnected)
        std::string  read         (unsigned limit=0);
        void         read         (std::string & buffer, unsigned limit=0);
        unsigned     read         (char * buffer, unsigned size);

        // read from unconnected socket returning peer address
        std::pair<std::string, Address> 
                     readfrom     ();
        void         readfrom     (std::string & buffer, Address & from);
        unsigned     readfrom     (char * buffer, unsigned size, Address & from);

        // write to connected socket
        unsigned     write        (std::string const & data);
        unsigned     write        (char const * buffer, unsigned size);

        // write to unconnected socket
        unsigned     writeto      (AddressParam addr, std::string const & data);
        unsigned     writeto      (AddressParam addr, char const * buffer, unsigned size);

        ///@}

        ///////////////////////////////////////////////////////////////////////////
        ///\name Addressing
        ///@{

        void         connect      (AddressParam addr);
        void         bind         (AddressParam addr);

        Address      peer         ();
        void         peer         (Address & addr);
        Address      local        ();
        void         local        (Address & addr);

        ///@}

        ///////////////////////////////////////////////////////////////////////////
        ///\name Buffering
        ///@{
        
        unsigned     rcvbuf      ();
        void         rcvbuf      (unsigned size);        
        unsigned     sndbuf      ();
        void         sndbuf      (unsigned size);
        
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
// End:
