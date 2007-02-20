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
    \brief SocketProtocol and ConcreteSocketProtocol public header

    \idea We should optimize the protocol handling. Allocating a protocol instance for every socket
        body seems quite wasteful. We could derive SocketPolicy from SocketBody (probably privately,
        since private inheritance models more of 'has a' than 'is a'). This would allow to reduce
        the number of heap-allocations per socket to one which is good.
 */

// The private inheritance idea should indeed work very well: We just need to chnage the
// implementations of body() and protocol() and that of the ProtocolClient/ServerSocketHandle
// constructors and the SocketBody constructor. The body and the protocol would still be visible
// like several instances because of the private inheritance but we would save the backwards
// pointer.

/** \defgroup protocol_group The Protocol Classes

    \image html Protocols.png

    The socket handle classes and templates only implement the most important socket API methods
    using the policy framework. To access the complete API, the protocol interface is
    provided. Access to the protocol interface is only possible via senf::ProtocolClientSocketHandle
    and senf::ProtocolServerSocketHandle which have the necessary \c protocol() member. This member
    returns a reference to the protocol class instance which contains members covering all the API
    functions (mostly setsockopt/getsockopt related calls but there may be more, this is completely
    up to the implementor of the protocol class) not found in the SocketHandle interface. The
    protocol interface is specific to the protocol. It's implementation is quite free. The standard
    protocols are implemented using a simple multiple-inheritance hierarchy as shown above.

    Since the protocol class is protocol specific (how intelligent ...), the protocol class also
    defines the complete socket policy to be used with it's protocol. Complete meaning, that every
    policy axis must be assigned it's the most specific (that is derived) policy class to be used
    with the protocol.

    \see
        \ref handle_group \n
        \ref policy_group

    \todo Complete the protocol interface implementations. Better distribution of members to
        protocol facets and more precise distribution of functionality among the facets.
 */

/** \defgroup concrete_protocol_group Protocol Implementations (Concrete Protocol Classes)
    \ingroup protocol_group

    Theese protocol classes define concrete and complete protocol implementations. They inherit from
    ConcreteSocketProtocol and are used with the ProtocolClientSocketHandle and
    ProtocolServerSocketHandle templates to instantiate socket handles. Appropriate typedefs are
    always provided.

    Every protocol defines both the protocol and the policy interface provided by that protocol. See
    the documentation of the protocol classes listed below for more information on the supported
    protocols. Every protocol class documents it's policy interface. Use the 'list all members' link
    of the protocol class to find the complete policy interface.
 */

/** \defgroup protocol_facets_group Protocol Facets
    \ingroup protocol_group

    The protocol facets are classes used as building blocks to build concrete protocol classes. Each
    protocol facet will implement some functional part of the protocol interface. The protocol
    facets all inherit from SocketProtocol by public \e virtual inheritance. This ensures the
    accessibility of the socket body from all facets.
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

    /** \brief Socket protocol base class

        This is the base class of all socket protocol classes. Every protocol class must directly or
        indirectly inherit from SocketProtocol

        \attention SocketProtocol must \e always be inherited using public \e virtual inheritance.
     */
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

        SocketBody & body() const;      ///< Access the socket body
                                        /**< \todo we don't need body(), we should better provide a
                                             handle() member which will return a simple FIleHandle
                                             object (we cannot return some other derived class since
                                             we don't know the Protocol or Policy at this point) */
        virtual SocketPolicyBase const & policy() const = 0;
                                        ///< Access the policy instance

        ///////////////////////////////////////////////////////////////////////////
        // Virtual interface

        virtual std::auto_ptr<SocketProtocol> clone() const = 0;
                                        ///< Polymorphically return a copy of this protocol class
                                        /**< This member will create a new copy of the protocol
                                             class on the heap.
                                             \attention This member must be implemented in every \e
                                                 leaf protocol class to return a new instance of the
                                                 appropriate type. */
        virtual unsigned available() const = 0;
                                        ///< Return number of bytes available for reading without
                                        ///< blocking
                                        /**< This member will check in a (very, sigh) protocol
                                             deqpendent way, how many bytes are guarateed to be
                                             readable from the socket without blocking even if the
                                             socket is blocking. */

        virtual bool eof() const = 0;   ///< Check for end-of-file condition
                                        /**< This is another check which (like available()) is
                                             extremely protocol dependent. This member will return
                                             \c true only, if at end-of-file. If the protocol does
                                             not support the notion of EOF, this member should
                                             always return \c false. */
        virtual void state(SocketStateMap & map, unsigned lod) const;
                                        ///< Return socket state information
                                        /**< This member is called to add state information to the
                                             status \a map. The protocol map should provide as
                                             detailed information as possible. The amount of
                                             information to be added to the map is selected by the
                                             \a lod value with a default value of 0. The
                                             interpretation of the \a lod value is completely
                                             implementation defined.

                                             Every class derived from SocketProtocol should
                                             reimplement state(). The reimplemented method should
                                             call (all) baseclass-implementations of this
                                             member.

                                             The \a map Argument is a map which associates
                                             std:string keys with std:string-like values. The map
                                             keys are interpreted as hierarchical strings with '.'
                                             as a separator (like hostnames or struct or class
                                             members). They are automatically sorted correctly.

                                             The values are std:string with one additional feature:
                                             they allow assignment or conversion from *any* type as
                                             long as that type is streamable. This simplifies
                                             assigning non-string values to the map:

                                             \code
                                               map["socket.protocol.ip.address"] = peer();
                                               map["socket.protocol.tcp.backlog"] = backlog();
                                             \endcode

                                             This will work even if peer() returns an ip-address
                                             object or backlog() returns an integer. The values are
                                             automatically converted to their string representation.

                                             The operator "+=" also has been reimplemented to
                                             simplify adding multiple values to a single entry: It
                                             will automatically add a ", " separator if the string
                                             is non-empty. */

    protected:

    private:
        // backpointer to owning SocketBody instance
        SocketBody * body_;
        friend class SocketBody;
   };


    /** \brief Concrete socket protocol implementation base class

        ConcreteSocketProtocol is the base class of a concrete socket protocol implementation. The
        final protocol class must inherit from ConcreteSocketProtocol. The template argument \a
        SocketPolicy must be set to the complete socket policy of the protocol.

        A protocol implementation may define the protocol interface directly. It can also
        (additnally) make use of multiple inheritance to combine a set of protocol facets into a
        specific protocol implementation (i.e. TCPv4SocketProtocol inherits from
        ConcreteSocketProtocol and from the protocol facets IPv4Protocol, TCPProtocol,
        BSDSocketProtocol and AddressableBSDSocketProtocol). The protocol facets are not concrete
        protocols themselves, they are combined to build concrete protocols. This structure will
        remove a lot of code duplication. It is important to ensure, that the protocol facets do not
        overlap, since otherwise there will be problems resolving overlapping members.
     */
    template <class SocketPolicy>
    class ConcreteSocketProtocol
        : public virtual SocketProtocol
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef SocketPolicy Policy;    ///< The protocols policy

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
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
