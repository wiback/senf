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
    \brief SocketHandle public header
 */

#ifndef HH_SENF_Socket_SocketHandle_
#define HH_SENF_Socket_SocketHandle_ 1

//#include "SocketHandle.mpp"
#include "SocketHandle.ih"

// Custom includes
#include <senf/Utils/Cpp11Support/smart_ptr.hh>
#include "FileHandle.hh"
#include "SocketPolicy.hh"

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /// \addtogroup handle_group
    //\{

    /** \brief basic SocketHandle supporting protocol and policy abstraction

        The %senf::SocketHandle class introduces the two abstraction layers of the socket
        library. senf::SocketHandle does \e not provide socket functions it only provides the
        infrastructure necessary to support both, the protocol and the policy interface.

        %SocketHandle takes the socket policy as a template argument. %SocketHandle also
        introduces the protocol class. However, the class has no public constructors (see the
        derived classes senf::ProtocolClientSocketHandle and senf::ProtocolServerSocketHandle).

        The most important functionality provided by %SocketHandle is the conversion
        constructor. This allows to implicitly convert between compatible socket handle types as
        specified by the socket policy. The conversion constructor is defined in such a way, that
        only valid conversions are possible (see the implementation source for a more complete
        discussion).

        \tparam SPolicy socket policy

        \note This class is \e not meant to be used as a base-class outside the library
        implementation; The protected interface is for internal use only.

        \todo Create a SocketHandleBase class and move some non-Policy dependent code there
     */
    template <class SPolicy>
    class SocketHandle
        : public FileHandle
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        typedef SPolicy Policy;

        /** \brief Check policy compatibility

            IsCompatible is a template meta-function which will check some other socket policy for
            conversion compatibility. This check is used in the senf::SocketPolicy implementation to
            restrict the conversion operator.
         */
        template <class OtherPolicy>
        struct IsCompatible
            : public boost::enable_if< SocketPolicyIsBaseOf<Policy,OtherPolicy>, SocketHandle >
        {};

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        // default default constructor
        // default copy constructor
        // default copy assignment
        // default destructor

        // here to implement
        SocketHandle();

        // conversion constructors

        template <class OtherPolicy>
        SocketHandle(SocketHandle<OtherPolicy> other,
                     typename IsCompatible<OtherPolicy>::type * = 0);
                                        ///< Convert from other socket handle checking policy
                                        ///< compatibility

        //\}
        //-////////////////////////////////////////////////////////////////////////

        template <class OtherPolicy>
        typename IsCompatible<OtherPolicy>::type const & operator=(SocketHandle<OtherPolicy> other);
                                        ///< Assign from other socket handle checking policy
                                        ///< compatibility

        void state(SocketStateMap & map, unsigned lod=0);
                                        ///< Inquire state information of socket handle
                                        /**< The map argument (a string to string mapping) will be
                                             filled with information covering the current state of
                                             the socket. The information provided depends on the
                                             socket protocol. The amount of information returned can
                                             be controlled using the \p lod value.

                                             See senf::SocketProtocol::state() for more information,
                                             how the Information is generated.

                                             \param map string to string mapping to be filled with
                                                 state information
                                             \param lod level of %detail requested. The
                                                 interpretation of this value is protocol specific

                                             \implementation This member will be re-implemented in
                                                 every derived class. This is very important since
                                                 state() is \e not a virtual function (which we
                                                 don't want since we don't want to add a vtable
                                                 pointer to every handle instance). */
        std::string dumpState(unsigned lod=0);
                                        ///< Format complete state information as string
                                        /**< Formats the complete state map value and returns it as
                                             a single multi-line string.

                                             \param lod level of %detail requested. The
                                                interpretation of this value is protocol specific

                                             \implementation This member will be re-implemented in
                                                 every derived class. See the state()
                                                 documentation. */

        template <class Facet>
        Facet & facet();                ///< Access a protocol facet
                                        /**< This member will try to access the given protocol facet
                                             of the socket. If \a Facet is a valid facet of the
                                             protocol, it is returned, otherwise \c std::bad_cast
                                             will be thrown.
                                             \throws std::bad_cast if \a Facet is not a protocol
                                                 facet of this socket
                                             \returns the \a Facet protocol facet of this socket */

    protected:
        explicit SocketHandle(std::unique_ptr<SocketBody> body);
                                        ///< Initialize SocketHandle providing the protocol
                                        /**< \param protocol Protocol class of the protocol
                                                 implemented by this socket handle
                                             \param isServer \c true, if this SobcketHandle instance
                                                 implements a server handle, \c false otherwise */
        SocketHandle(FileHandle other, bool isChecked);
                                        ///< Initialize SocketHandle from arbitrary checked
                                        ///< FileHandle
                                        /**< This constructor is used to support up- and downcasting
                                             of SocketHandle instances.

                                             \warning It is absolutely necessary to ensure, that the
                                                 FileHandle passed in is \e really a SocketHandle
                                                 holding a SocketBody (and not a simple FileBody)
                                                 instance. Additionally. the SocketPolicy absolutely
                                                 must be compatible.

                                             \param other FileHandle to assign
                                             \param isChecked has to be \c true

                                             \todo Answer, why the heck I need the \c isChecked
                                                 parameter ?? */

        SocketBody & body();            ///< Access socket body
                                        /**< This member replaces the corresponding FileHandle
                                             member and returns an appropriately cast body
                                             reference */
        SocketBody const & body() const; ///< Access socket body in const context
                                        /**< This member replaces the corresponding FileHandle
                                             member and returns an appropriately cast body
                                             reference */
        SocketProtocol & protocol() const;
                                        ///< Access protocol class

        void assign(FileHandle other);  /**< \internal */

    public:
        static SocketHandle cast_static(FileHandle handle);
                                        /**< \internal */
        static SocketHandle cast_dynamic(FileHandle handle);
                                        /**< \internal */

    private:

    };

    /** \brief Write stream status dump to output stream

        Write senf::SocketHandle::dumpState() to \c os

        \related senf::SocketHandle
     */
    template <class SPolicy>
    std::ostream & operator<<(std::ostream & os, SocketHandle<SPolicy> handle);

    /** \brief static socket (down-)cast

        This function is like \c static_cast but for socket handles. It allows to downcast any
        FileHandle to any SocketHandle (and its derived types). static_socket_cast will \e not check
        the validity of the cast, it will assume, that the cast is valid.

        The function will however check, that the cast is possible. Casting between (at compile
        time) known incompatible types (like casting a SocketHandle with a communication policy of
        ConnectedCommunicationPolicy to a SocketHandle with UnconnectedCommunicationPolicy will fail
        at compile time).

        \warning
        If the type you cast to is not really a compatible socket handle type you will get undefined
        behavior, probably your program will crash (You will get an assertion in debug builds).

        \related senf::SocketHandle
     */
    template <class Target, class Source>
    Target static_socket_cast(Source handle);

    /** \brief dynamic socket (down-)cast

        This function is like \c dynamic_cast but for socket handles. It is a runtime typechecked
        version of static_socket_cast.

        \throws std::bad_cast You have tried to perform an invalid down- or crosscast.

        \related senf::SocketHandle
     */
    template <class Target, class Source>
    Target dynamic_socket_cast(Source handle);

    /** \brief dynamically check cast validity

        This function will check, whether the given cast is valid. This is the same as checking, that
        dynamic_socket_cast does not throw.

        This member is needed, since there is no 'null' SocketHandle (comparable to a null pointer)
        which could be returned by a non-throwing variant of dynamic_socket_cast.

        \related senf::SocketHandle
     */
    template <class Target, class Source>
    bool check_socket_cast(Source handle);

    //\}
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "SocketHandle.cci"
#include "SocketHandle.ct"
#include "SocketHandle.cti"
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
