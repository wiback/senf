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

    \idea We should optimize the protocol handling. Allocating a
	protocol instance for every socket body seems quite
	wasteful. However I have no idea, how to access the socket
	handle from within the protocol interface if the protocol
	class is a singleton. Maybe, we'll have some other idea to
	reduce the number of heap allocations (like deriving the
	SocketProtocol class (private? protected?) from the
	SocketBody. (private inheritance is a form of 'has-a' instead
	of 'is-a' which would fit very well here). This would allow to
	reduce the number of heap-allocations per socket to one which
	is good.
 */

/** \defgroup protocol_group The Protocol Classes

    \image html Protocols.png

    The socket handle classes and templates only implement the most
    important socket API methods using the policy framework. To access
    the complete API, the protocol interface is provided. Access to
    the protocol interface is only possible via
    senf::ProtocolClientSocketHandle and
    senf::ProtocolServerSocketHandle which have the necessary \c
    protocol() member. This member returns a reference to the protocol
    class instance which contains members covering all the API
    functions (mostly setsockopt/getsockopt related calls but there
    may be more, this is completely up to the implementor of the
    protocol class) not found in the SocketHandle interface. The
    protocol interface is specific to the protocol. It's
    implementation is quite free. The standard protocols are
    implemented using a simple multiple-inheritance hierarchy as shown
    above.

    Since the protocol class is protocol specific (how intelligent
    ...), the protocol class also defines the complete socket policy
    to be used with it's protocol. Complete meaning, that every policy
    axis must be assigned it's the most specific (that is derived)
    policy class to be used with the protocol.
 */

#ifndef HH_SocketProtocol_
#define HH_SocketProtocol_ 1

// Custom includes
#include <boost/utility.hpp>
/** \fixme this is not nice. The includes and predefs should be restructured */
#include "SocketHandle.ih"

//#include "SocketProtocol.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup protocol_group
    /// @{

    class SocketPolicyBase;

    class SocketProtocol : boost::noncopyable
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        SocketProtocol();
        virtual ~SocketProtocol() = 0;

        // default default constructor
        // no copy
        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        SocketBody & body() const;
        virtual SocketPolicyBase const & policy() const = 0;
        
        ///////////////////////////////////////////////////////////////////////////
        // Virtual interface

        virtual std::auto_ptr<SocketProtocol> clone() const = 0;
        virtual unsigned available() const = 0;
        virtual bool eof() const = 0;
        virtual void state(SocketStateMap & map, unsigned lod) const;

    protected:

    private:
        // backpointer to owning SocketBody instance
        SocketBody * body_;
        friend class SocketBody; 
   };

    template <class SocketPolicy>
    class ConcreteSocketProtocol
        : public virtual SocketProtocol
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef SocketPolicy Policy;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        ~ConcreteSocketProtocol() = 0;

        // no default constructor
        // no copy
        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        Policy const & policy() const;

    protected:

    private:
        Policy policy_;

    };

    /// @}
}

///////////////////////////////hh.e////////////////////////////////////////
#include "SocketProtocol.cci"
//#include "SocketProtocol.ct"
#include "SocketProtocol.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "senf"
// End:
