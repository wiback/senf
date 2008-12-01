// $Id:SocketPolicy.hh 218 2007-03-20 14:39:32Z tho $
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
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
    \brief Policy Framework public header

    \idea Creating a new Socket will create 3 new instances (The handle, the body, the policy) of
        which 2 (argh) (body, policy) live on the heap. This is expensive. We should convert all the
        policy classes to singletons and assign the same instance to all socket bodies with the same
        policy. This would reduce the number of heap allocations per socket handle to one (which is
        already optimized using the pool_alloc_mixin)
 */

/** \defgroup policy_group The Policy Framework

    <div class="diamap" name="SocketPolicy">
    <span coords="39,229,182,257">\ref WritePolicyBase</span>
    <span coords="17,76,210,105">\ref AddressingPolicyBase</span>
    <span coords="29,114,194,143">\ref FramingPolicyBase</span>
    <span coords="368,0,463,28">\ref PolicyBase</span>
    <span coords="275,2,691,185">\ref SocketPolicy</span>
    <span coords="0,152,227,181">\ref CommunicationPolicyBase</span>
    <span coords="41,191,180,219">\ref ReadPolicyBase</span>
    </div>
    \htmlonly <img src="SocketPolicy.png" border="0" alt="SocketPolicy" usemap="#SocketPolicy"> \endhtmlonly

    \section policy_group_introduction Introduction to the Policy Framework

    The policy framework conceptually implements a list of parallel inheritance hierarchies each
    covering a specific interface aspect of the socket handle. The socket handle itself only
    provides minimal functionality. All further functionality is relayed to a policy class, or more
    precisely, to a group of policy classes, one for each policy axis. The policy axis are

    <dl><dt><em>addressingPolicy</em></dt><dd>configures, whether a socket is addressable and if
    so, configures the address type</dd>

    <dt><em>framingPolicy</em></dt> <dd>configures the type of framing the socket provides: either
    no framing providing a simple i/o stream or packet framing</dd>

    <dt><em>communicationPolicy</em></dt><dd>configures,if and how the communication partner is
    selected</dd>

    <dt><em>readPolicy</em></dt><dd>configures the readability of the socket</dd>

    <dt><em>writePolicy</em></dt><dd>configures the writability of the socket</dd></dl>

    The template senf::SocketPolicy combines these policy axis to form a concrete socket policy. In
    a concrete policy, each of these policy axis is assigned a value, the policy value. This value
    is identified by a class type, a policy class. E.g. possible values for <em>framingPolicy</em>
    are <tt>DatagramFramingPolicy</tt> or <tt>StreamFramingPolicy</tt> which are classes derived
    from the axis base class <tt>FramingPolicyBase</tt>. This base class also doubles as
    <tt>UnspecifiedFramingPolicy</tt> (which is just a typedef alias).  If a policy axis is assigned
    this Unspecified type, the axis is left unspecified, the concrete policy will be incomplete.

    The senf::SocketPolicy template defines the behavior of a socket handle. The socket handle
    instances do not implement any socket functionality themselves instead deferring the
    implementation to the policy classes. The SocketHandle interface is therefore \e not implemented
    using virtual members, all important socket functions can be inlined by the compiler to create
    highly efficient code.

    A senf::SocketPolicy instance can be incomplete. In this case it does \e not completely specify
    the socket interface, it leaves some aspects open by assigning the Unspecified value to one or
    more of the policy axis. A senf::SocketHandle based on such a policy will have a reduced
    interface: It will only support those members for which the corresponding policies are defined.

    To build a senf::SocketPolicy instance the senf::MakeSocketPolicy helper is provided. This
    helper template takes any number (it is really limited to 6 Arguments but more arguments don't
    make sense) of policy classes as it's argument. The MakeSocketPolicy helper will take the
    arguments in the order they are specified and for each argument will check to which axis the
    policy class belongs (by checking the base classes of that class) and assign it to the correct
    policy axis in the senf::SocketPolicy template. If any policy axis are not specified, they are
    defaulted to their corresponding Unspecified value. This helper frees you to specify the policy
    classes in any order. An additional feature is, that you may specify a complete policy as a
    first argument. This policy will then be used to provide default values for unspecified axis.

    Two senf::SocketHandle's with different policies can be \e compatible. If they are, the more
    specific SocketHandle can be converted (assigned to) the more basic SocketHandle. A SocketHandle
    is more specific then another SocketHandle if the policy of the former is more specific then
    that of the latter which means, that for each policy axis separately, the value of that axis of
    the more specific policy is derived from or the same as the value of that axis in the more basic
    policy. This is like converting a derived class pointer to a base class pointer, only it happens
    separately but at the same time for each policy axis:

    \code
    // This defines an incomplete policy where addressingPolicy and writePolicy
    // are unspecified
    typedef senf::MakeSocketPolicy<
        senf::StreamFramingPolicy,
        senf::ConnectedCommunicationPolicy,
        senf::ReadablePolicy
        >::policy MyReadableSocketPolicy

    typedef senf::ClientSocketHandle<MyReadableSocketPolicy> MyReadableHandle;

    // TCPv4ClientSocketHandle is a socket handle with the policy equivalent to
    // senf::MakeSocketPolicy<
    //     INet4AddressingPolicy,
    //     StreamFramingPolicy,
    //     ConnectedCommunicationPolicy,
    //     ReadablePolicy,
    //     WritablePolicy>::policy
    senf::TCPv4ClientSocketHandle tcpHandle (...);

    MyReadableHandle myHandle (tcpHandle); // Conversion to more basic socket handle
    \endcode

    \section policy_group_details The Policy Framework Classes

    In the following discussion, deeper insight into C++ and especially the concepts of template
    meta-programming are needed. However, this information is only needed if you want to write new
    policy classes or want to use the policy framework explicitly for your own involved
    optimizations ... or if you are just plain curious :-)

    In the following discussion we will use the following conventions:
    \li \e Axis is one or \c AddressingPolicy, \c FramingPolicy, \c CommunicationPolicy, \c
        ReadPolicy or \c WritePolicy
    \li \e socketPolicy is any socket policy (that is, an instantiation of the SocketPolicy
        template)
    \li \e trait is an any policy class (that is, any class derived from one of the axis base
        classes)

    Each axis is comprised of a number of classes and templates (all in namespace senf of course):

    <dl><dt>\e Axis \c Base (ex: AddressingPolicyBase)</dt><dd>Baseclass of all policies in this
    axis</dd>

    <dt>\c Unspecified \e Axis (ex: \ref UnspecifiedAddressingPolicy)</dt> <dd>An alias (typedef)
    for \e Axis \c Base</dd>

    <dt>\e Axis \c Is < \e socketPolicy, \e trait > (ex: AddressingPolicyIs)</dt> <dd>A template
    metafunction returning \c boost::true_type, if \e trait (any class derived from \e Axis \c
    Base) is a compatible policy value of the given \e socketPolicy</dd>

    <dt>\c If \e Axis \c Is < \e socketPolicy, \e trait > (ex: IfAddressingPolicyIs)</dt> <dd>This
    is a combination of \e Axis \c Is and \c boost::enable_if</dd>

    <dt>\c If \e Axis \c IsNot < \e socketPolicy, \e trait > (ex: IfAddressingPolicyIsNot)</dt>
    <dd>The inverse of above</dd> </dl>

    These classes form the basis of the policy framework. To bind the policy axis together, there
    are some more classes and templates.

    <dl><dt>\c class \c SocketPolicyBase</dt> <dd>This class is the base class of the SocketPolicy
    template. It is used to validate, that a class is really a SocketPolicy (by checking, that it
    derives from SocketPolicyBase. This is simpler than checking the template directly).</dd>

    <dt>\c template \c SocketPolicy < \e addressingPolicy, \e framingPolicy, \e communicationPolicy,
    \e readPolicy, \e writePolicy ></dt> <dd>This is the central SocketPolicy
    template. It combines a complete set of policy classes, one for each axis.</dd>

    <dt>\c template \c MakeSocketPolicy < \e args ></dt> <dd>\c MakeSocketPolicy is a template
    metafunction which simplifies building SocketPolicy instantiations. It takes any number (ok, up
    to a maximum of 6) of policy classes as an argument (in any order). It will sort these arguments
    into the SocketPolicy template arguments. If for some axis no class is specified, it's slot will
    be filled with \c Unspecified \e Axis. Additionally, the first Argument may optionally be ab
    arbitrary SocketPolicy. It will provide default values for unspecified axis</dd>

    <dt>\c template \c SocketPolicyIsBaseOf < \e base, \e derived ></dt> <dd>This template
    metafunction will check, whether the socket policy \e derived is convertible to \e base. This
    means, that for each axis, the corresponding policy class in \e derived must be derived or be
    the same as the one on \e base.</dd> </dl>

    \implementation All these classes are created automatically. The \c SENF_SOCKET_POLICIES macro
    is a Boost.Preprocessor style sequence listing all policy axis. The Boost.Preprocessor library
    is then used to generate the respective classes.

    \section policy_implement Implementing Policy Classes

    To define a new policy class, derive from the corresponding base class for your policy axes. The
    only policy axis which might possibly need to be extended is the addressing policy
    (AddressingPolicyBase). See the Documentation of these classes for more information on which
    members can be implemented.

    All members you define must be static. For any of the policy classes, you must only define those
    members which are supported by your implementation. If you leave out a member you automatically
    disable the corresponding functionality in the ClientSocketHandle/ServerSocketHandle interface.

    The member prototypes given in the base class documentation only specify the call signature not
    the way, the member must be defined (FileHandle really is not a FileHandle but an arbitrary
    SocketHandle).

    If the existence of a member depends on other policies, you should use the
    <code>If</code><i>SomePolicy</i><code>Is</code> and
    <code>If</code><i>SomePolicy</i><code>IsNot</code> templates to dynamically enable/disable the
    member depending on some other policy:

    \code
      struct ExampleAddressingPolicy
      {
          template <class SPolicy>
          void connect(senf::SocketHandle<SPolicy> handle, Address & addr,
                       typename senf::IfCommmunicationPolicyIs<
                           SPolicy, senf::ConnectedCommunicationPolicy>::type * = 0);
      };
    \endcode

    The \c connect member in this example will only be enabled, it the communication policy of the
    socket handle is ConnectedCommunicationPolicy (or a derived type). See <a
    href="http://www.boost.org/libs/utility/enable_if.html">Boost.Enable_If</a> for a discussion of
    the third argument (\c senf::ConnectedCommunicationPolicyIs is based on the \c boost::enable_if
    template).

    \see \ref extend_policy \n
         <a href="http://www.boost.org/libs/utility/enable_if.html">The Boost enable_if utility</a> \n
         <a href="http://www.boost.org/libs/mpl/doc/index.html">The Boost.MPL library</a> \n
         <a href="http://www.boost.org/libs/preprocessor/doc/index.html">The Boost.Preprocessor library</a>

    \idea We could combine all the \e Axis \c Is templates into a single template. Since the \e
    trait argument will automatically specify the axis to be used, it is not necessary to specify
    that axis in the template functor's name. We could even combine this with \c
    SocketPolicyIsBaseOf.
 */

/** \defgroup policy_impl_group Policy Implementation classes
    \ingroup policy_group

    Here you will find all policy classes. Also included are some supporting classes which are used
    as base classes to build other policy classes.
 */

#ifndef HH_SENF_Socket_SocketPolicy_
#define HH_SENF_Socket_SocketPolicy_ 1

// Custom includes

//#include "SocketPolicy.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup policy_group
    /// @{

    // This may be adapted to change the supported policies (however, ClientSocketHandle and
    // ServerSocketHandle will probably have to be adjusted accordingly)

    /** \brief List all policy axis

        \internal

        This define symbol is used to configure the policy axis. The base class for each of these
        axis must be defined explicitly (e.g. AddressingPolicyBase). The implementation files will
        then automatically generate all the other classes from this list.

        \see policy_group
     */
#   define SENF_SOCKET_POLICIES                 \
        (AddressingPolicy)                      \
        (FramingPolicy)                         \
        (CommunicationPolicy)                   \
        (ReadPolicy)                            \
        (WritePolicy)

    // Wer define these classes explicitly (and not with some macro
    // magic) because
    // a) AddressingPolicyBase is different from all the others
    // b) We want to document each one explicitly

    /** \brief Policy defining socket addressing

        AddressingPolicyBase is the baseclass of all addressing policy classes. When defining a new
        addressing policy, the following members can be defined. All methods must be static.

        <table class="senf">
        <tr><td>typedef</td> <td><tt>Address</tt></td>                                   <td>Address type</td></tr>
        <tr><td>method</td>  <td><tt>void local(FileHandle, Address &)</tt></td>         <td>Get local socket address</td></tr>
        <tr><td>method</td>  <td><tt>void peer(FileHandle, Address &)</tt></td>          <td>Get remote socket address</td></tr>
        <tr><td>method</td>  <td><tt>void bind(FileHandle, Address const &)</tt></td>    <td>Bind socket to local address</td></tr>
        <tr><td>method</tr>  <td><tt>void connect(FileHandle, Address const &)</tt></td> <td>Connect to remote address</td></tr>
        </table>

        \see policy_group
     */
    struct AddressingPolicyBase
    {
        virtual ~AddressingPolicyBase();

        class Address { Address(); };
    };

    /** \brief Policy defining the framing format

        This policy does not define any operations since it does have no influence on any method
        signature. It does however affect the semantics of the \c read() and \c write() operations.

        \note This policy axis probably only has two sensible states: StreamFramingPolicy and
        DatagramFramingPolicy.

        \see policy_group
     */
    struct FramingPolicyBase
    {
        virtual ~FramingPolicyBase();
    };

    /** \brief Policy defining, how peers are selected

        The CommunicationPolicy may define two members:

        <table class="senf">
        <tr><td>method</td> <td><tt>void listen(FileHandle, unsigned backlog)</tt></td> <td>Switch socket into listening state</td></tr>
        <tr><td>method</td> <td><tt>int accept(FileHandle, Address &)</tt></td>         <td>Accept a new connection</td></tr>
        <tr><td>method</td> <td><tt>int accept(FileHandle)</tt></td>                    <td>Accept a new connection</td></tr>
        </table>

        The \c listen member is straight forward. The \c accept() member must return a new file
        descriptor (which will be used to create a new SocketHandle of the correct
        type).

        \note This Policy only has two meaningful states: ConnectedCommunicationPolicy and
        UnconnectedCommunicationPolicy. It is probably not sensible to define a new
        CommunicationPolicy type.

        \see policy_group
     */
    struct CommunicationPolicyBase
    {
        virtual ~CommunicationPolicyBase();
    };

    /** \brief Policy defining the readability

        The ReadPolicy defines, whether the socket is readable. It may define two members:

        <table class="senf">
        <tr><td>method</td> <td><tt>unsigned read(FileHandle, char * buffer, unsigned size)</tt></td>                <td>read data from socket</td></tr>
        <tr><td>method</td> <td><tt>unsigned readfrom(FileHandle, char * buffer, unsigned size, Address &)</tt></td> <td>read data from unconnected socket</td></tr>
        </table>

        The second member should only be enabled if the communication policy is
        UnconnectedCommunicationPolicy (otherwise it does not make sense since the communication partner
        is fixed) (see AddressingPolicyBase on how to do this).

        \note This Policy only has two meaningful states: ReadablePolicy and NotReadablePolicy. It
        probably does not make sense to define new read policy types.

        \see policy_group
     */
    struct ReadPolicyBase
    {
        virtual ~ReadPolicyBase();
    };

    /** \brief Policy defining the writability

        The WritePolicy defines, whether the socket is writable. It may define two members:

        <table class="senf">
        <tr><td>method</td> <td><tt>unsigned write(FileHandle, char * buffer, unsigned size)</tt></td>              <td>read data from socket</td></tr>
        <tr><td>method</td> <td><tt>unsigned writeto(FileHandle, char * buffer, unsigned size, Address &)</tt></td> <td>read data from unconnected socket</td></tr>
        </table>

        The second member should only be enabled if the communication policy is
        UnconnectedCommunicationPolicy (otherwise it does not make sense since the communication partner
        is fixed) (see AddressingPolicyBase on how to do this).

        \note This Policy only has two meaningful states: WritablePolicy and NotWritablePolicy. It
        probably does not make sense to define new write policy types.

        \see policy_group
     */
    struct WritePolicyBase
    {
        virtual ~WritePolicyBase();
    };

    // The implementation file will for each Policy declared above
    // define the following (SomePolicy is one of the above):
    //
    // struct SomePolicyBase;
    // typedef UndefinedSomePolicy;
    // template SomePolicyIs< SocketPolicy, Axis >
    // template IfSomePolicyIs< SocketPolicy, Axis >
    // template IfSomePolicyIsNot< SocketPolicy, Axis >
    //
    // Additionally the following are defined:
    //
    // class SocketPolicyBase
    // template SocketPolicy< ..policies.. >
    // template MakeSocketPolicy< ..args.. >
    // template SocketPolicyIsBaseOf< Base, Derived >

#   ifdef DOXYGEN

    // The following stub definitions are only visible to doxygen

    /** \brief Alias of AddressingPolicyBase for better readability
        \see \ref policy_group
     */
    typedef AddressingPolicyBase UnspecifiedAddressingPolicy;

    /** \brief Check single policy axis

        This template is an example of the \e Axis \c Is family of template metafunctions. It will
        check, whether \c Trait is a valid compatible Policy class of \c SocketPolicy. \c Trait must
        be derived from AddressingPolicyBase (respectively \c Policy \c Base).

        \see \ref policy_group
     */
    template <class SocketPolicy, class Trait>
    struct AddressingPolicyIs
    {};

    /** \brief Enable template overload depending on policy value

        This template is an example of the \c If \e Axis \c Is family of templates. It is used like
        <a href="http://www.boost.org/libs/utility/enable_if.html">Boost.enable_if</a> to enable a
        templated overload only, if the AddressingPolicy of \e Axis is compatible with \c Trait
        (that is the AddressingPolicy of \c Policy is derived from \c Trait).

        \see policy_group
     */
    template <class SocketPolicy, class Trait>
    struct IfAddressingPolicyIs
    {};

    /** \brief Inversion of \c IfAddressingPolicyIs
        \see policy_group
     */
    template <class SocketPolicy, class Trait>
    struct IfAddressingPolicyIsNot
    {};

    /** \brief Baseclass of all SocketPolicies

        \internal

        This class provides the baseclass of all socket policies (bundles). It serves two purposes:
        \li It allows us to easily identify a socket policy bundle by checking a classes baseclass.
        \li It provides an abstract (virtual) interface to access the policy axes

        \see policy_group
     */
    struct SocketPolicyBase
    {
        /** \brief Polymorphic access to policy axes

            This is an example of a policy axes accessor. It returns a reference to the policy axes
            used by the concrete protocol bundle. This reference can then be checked using RTTI
            information.
         */
        AddressingPolicyBase const & theAddressingPolicy() const = 0;

        virtual ~SocketPolicyBase();
    };

    /** \brief Collection of policy classes

        The SocketPolicy template defines the complete Policy used by the socket library. It
        contains one policy class for each policy axis. This template takes one policy from each
        axis as it's template arguments (this example implementation only has AddressingPolicy as an
        argument).

        A SocketPolicy can be complete or incomplete. An incomplete SocketPolicy will have at least
        one axis set to \c Undefined \e Axis (or a generic derived class which is used to group some
        other policies but does not (completely) define the policy behavior). A complete
        SocketPolicy will have a concrete definition of the desired behavior for each policy axis.

        \see policy_group
     */
    template < class AddressingPolicy >
    struct SocketPolicy
    {
        /** \brief Check dynamic policy compatibility

            This check will validate, that a socket with \a other as it's policy is convertible to a
            socket with the current SocketPolicy as it's policy. This is true, if for each policy
            axis, the policy class of that axis as defined in the \a other policy is convertible to
            the policy class of that same axis in the current SocketPolicy instance (as is defined
            by the template arguments). This again is true, if the \a other policy class is derived
            from (or is the same as) the policy class taken from the current SocketPolicy instance.

            In other words, this call checks, that the current SocketPolicy (as defined via the
            template arguments) is more generic than the \a other socket policy.

            \param[in] other SocketPolicy to check
            \throws std::bad_cast if \a other is not a compatible policy
         */
        static void checkBaseOf(SocketPolicyBase const & other);
    };

    /** \brief Metafunction to create SocketPolicy

        This template metafunction simplifies the creation of a SocketPolicy instantiation. It takes
        any number (that is up to 6) of Policy classes as arguments in any Order. It will create a
        SocketPolicy from these policy classes. Any axis not specified will be left as \c
        Unspecified \e Axis.

        \see policy_group
     */
    template <class Arg1, class Arg2, class ArgN>
    struct MakeSocketPolicy
    {};

    /** \brief Check policy compatibility

        This template metafunction checks, whether the SocketPolicy \c Derived is more specialized
        than \c Base (and therefore a SocketHandle with policy \c Derived is convertible to a
        SocketHandle with policy \c Base).

        The metafunction will return true (that is inherits from \c boost::true_type, see the <a
        href="http://www.boost.org/libs/mpl/doc/index.html">Boost.MPL</a> library documentation for
        more information) if each policy class in \c Base is a baseclass of (or the same as) the
        corresponding policy class in \c Derived.

        \see policy_group
     */
    template <class Base, class Derived>
    struct SocketPolicyIsBaseOf
    {};

#   endif

    /// @}
}

//////////////////////////////hh.e////////////////////////////////////////
#include "SocketPolicy.ih"
//#include "SocketPolicy.cci"
#include "SocketPolicy.ct"
//#include "SocketPolicy.cti"
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
