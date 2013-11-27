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
    \brief SocketProtocol and ConcreteSocketProtocol public header
 */

// The private inheritance idea should indeed work very well: We just need to change the
// implementations of body() and protocol() and that of the ProtocolClient/ServerSocketHandle
// constructors and the SocketBody constructor. The body and the protocol would still be visible
// like several instances because of the private inheritance but we would save the backwards
// pointer.

/** \defgroup protocol_group The Protocol Classes

    \diaimage Protocols.dia

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
    defines the \e complete socket policy to be used with it's protocol. Complete meaning, that
    every policy axis must be assigned it's the most specific (that is derived) policy class to be
    used with the protocol and that no policy axis is allowed to be left unspecified.

    \see
        \ref handle_group \n
        \ref policy_group
 */

/** \defgroup concrete_protocol_group Protocol Implementations (Concrete Protocol Classes)
    \ingroup protocol_group

    These protocol classes define concrete and complete protocol implementations. They inherit from
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

#ifndef HH_SENF_Socket_SocketProtocol_
#define HH_SENF_Socket_SocketProtocol_ 1

// Custom includes
#include <boost/utility.hpp>
// Hrmpf ... I have tried very hard, but I just can't find a nice, generic way to clean
// up this include
#include "SocketHandle.ih"

//#include "SocketProtocol.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /// \addtogroup protocol_group
    //\{

    struct SocketPolicyBase;

    /** \brief Socket Protocol base class

        This is the base class of all socket protocol classes. Every protocol class must directly or
        indirectly inherit from SocketProtocol

        \attention SocketProtocol must \e always be inherited using public \e virtual inheritance.
     */
    class SocketProtocol
        : boost::noncopyable
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        SocketProtocol();
        virtual ~SocketProtocol() = 0;

        // default default constructor
        // no copy
        // no conversion constructors

        //\}
        //-////////////////////////////////////////////////////////////////////////

        virtual SocketPolicyBase const & policy() const = 0;
        ///< Access the policy instance

        //-////////////////////////////////////////////////////////////////////////
        // Virtual interface

        virtual unsigned available() const = 0;
                                        ///< Return (maximum) number of bytes available for reading
                                        ///< without < blocking
                                        /**< This member will check in a (very, sigh) protocol
                                             dependent way, how many bytes may be read from a socket
                                             in a single (non-blocking) read operation. If the
                                             socket does not support reading (viz. NotReadablePolicy
                                             is set), this member should always return \c 0.

                                             Depending on the protocol, it may not be possible to
                                             return a good value. In this case, an upper bound may
                                             be returned (e.g.: When reading from a socket which
                                             returns ethernet frames, returning 1500 from
                                             available() is ok). However, this should only be done
                                             as a last resort. Also beware, that this number should
                                             not be too large since the socket layer will always
                                             need to allocate that number of bytes for the data to
                                             be read. */

        virtual bool eof() const = 0;   ///< Check for end-of-file condition
                                        /**< This is another check which (like available()) is
                                             extremely protocol dependent. This member will return
                                             \c true only, if at end-of-file. If the protocol does
                                             not support the notion of EOF, this member should
                                             always return \c false. */

        virtual void close();           ///< Close socket
                                        /**< This override will automatically \c shutdown() the
                                             socket whenever it is closed.
                                             \throws senf::SystemException
                                             \fixme Move into (at least) BSDSOcketProtocol */

        virtual void terminate() const; ///< Forcibly close socket
                                        /**< This override will automatically \c shutdown() the
                                             socket whenever it is called. Additionally it will
                                             disable SO_LINGER to ensure, that v_terminate will not
                                             block. Like the overridden method, this member will
                                             ignore failures and will never throw. It is therefore
                                             safe to be called from a destructor.
                                             \fixme Move into (at least) BSDSocketProtocol */

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
                                                 map["socket.protocol.ip.address"] << peer();
                                                 map["socket.protocol.tcp.backlog"] << backlog();
                                             \endcode

                                             This will work even if peer() returns an ip-address
                                             object or backlog() returns an integer. The values are
                                             automatically converted to their string representation.

                                             Additionally, if the slot the date is written to is not
                                             empty, the <tt>\<\<</tt> operator will add add a comma
                                             as separator. */

    protected:
        FileHandle fh() const;          ///< Get a FileHandle for this instance
                                        /**< This member will re turn a FileHandle instance for this
                                             protocol instance. You may cast this FileHandle
                                             instance to a ClientSocketHandle / ServerSocketHandle
                                             as long as you know some of the socket policy using
                                             static_socket_cast or dynamic_socket_cast */

        int fd() const;                 ///< Get file descriptor
                                        /**< Returns the file descriptor this protocol instance
                                             references. This is the same as <tt>fh().fd()</tt> but
                                             is implemented here since it is needed so often. */

        void fd(int) const;             ///< Initialize file descriptor
                                        /**< Assigns the file descriptor to the file handle, this
                                             protocol instance references. Only valid, if the file
                                             handle has not yet been assigned any descriptor (To
                                             change the file descriptor association later, use \c
                                             ::dup2()). */

    private:
        virtual SENF_SMART_PTR<SocketBody> clone(bool isServer) const = 0;
        virtual SENF_SMART_PTR<SocketBody> clone(int fd, bool isServer) const = 0;
        virtual SocketBody & body() const = 0;

        friend class SocketBody;
    };

    template <class SPolicy> class ClientSocketHandle;
    template <class SPolicy> class ServerSocketHandle;

    /** \brief Concrete Socket Protocol implementation base class

        ConcreteSocketProtocol is the base class of a concrete socket protocol implementation. The
        final protocol class must inherit from ConcreteSocketProtocol. The template argument \a
        SocketPolicy must be set to the complete socket policy of the protocol. \a Self is the name
        of the final protocol class which inherits this class.

        A protocol implementation may define the protocol interface directly. It can also
        (additionally) make use of multiple inheritance to combine a set of protocol facets into a
        specific protocol implementation (i.e. TCPv4SocketProtocol inherits from
        ConcreteSocketProtocol and from the protocol facets IPv4SocketProtocol, TCPSocketProtocol,
        BSDSocketProtocol and AddressableBSDSocketProtocol). The protocol facets are not concrete
        protocols themselves, they are combined to build concrete protocols. This structure will
        remove a lot of code duplication. It is important to ensure, that the protocol facets do not
        overlap, since otherwise there will be problems resolving overlapping members.

        \doc init_client init_server
     */
    template <class SocketPolicy, class Self>
    class ConcreteSocketProtocol
        : public virtual SocketProtocol
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        typedef SocketPolicy Policy;    ///< The protocols policy

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        ~ConcreteSocketProtocol() = 0;

        // no default constructor
        // no copy
        // no conversion constructors

        //\}
        //-////////////////////////////////////////////////////////////////////////

        Policy const & policy() const;

    protected:
        ClientSocketHandle<Policy> clientHandle() const;
                                        ///< Get client handle for associated socket
                                        /**< Returns a client handle for the socket associated with
                                             this protocol instance */
        ServerSocketHandle<Policy> serverHandle() const;
                                        ///< Get server handle for associated socket
                                        /**< Returns a server handle for the socket associated with
                                             this protocol instance */

    private:
        virtual SENF_SMART_PTR<SocketBody> clone(bool isServer) const;
        virtual SENF_SMART_PTR<SocketBody> clone(int fd, bool isServer) const;
        virtual SocketBody & body() const;

        Policy policy_;
    };

    //\}
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
// compile-command: "scons -u test"
// comment-column: 40
// End:
