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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

/** \file
    \brief Policy Framework public header

    \todo We should probably remove BufferingPolicy from the
	interface, it does not make much sense (how did I come to
	include it ??)
    
    \todo Do we want to support separate read and write policies. This
	allows to treat pipes within this framework however, is this
	worth the effort?

    \idea Creating a new Socket will create 4 (!) new instances (The
	handle, the body, the policy and the protocol) of which 3
	(argh) (body, policy and protocol) live on the heap. This is
	expensive. We should convert all the policy classes to
	singletons and assign the same instance to all socket bodies
	with the same policy. This would reduce the number of heap
	allocations per socket handle to two.
 */

/** \defgroup policy_group The Policy Framework

    \image html SocketPolicy.png

    \section policy_group_introduction Introduction to the Policy Framework

    The policy framework conceptually implements a list of parallel
    inheritance hierarchies each covering a specific interface aspect
    of the socket handle. The socket handle itself only provides
    minimal functionality. All further functionality is relayed to a
    policy class, or more precisely, to a group of policy classes, one
    for each policy axis. The policy axis are
    
    <dl>
    <dt><em>addressingPolicy</em></dt>
    <dd>configures, whether a socket is
    addressable and if so, configures the address type</dd>
    
    <dt><em>framingPolicy</em></dt>
    <dd>configures the type of framing the socket provides: either no
    framing providing a simple i/o stream or packet framing</dd>
    
    <dt><em>communicationPolicy</em></dt>
    <dd>configures,if and how the communication partner is
    selected</dd> 
    
    <dt><em>readPolicy</em></dt>
    <dd>configures the readability of the socket</dd>
    
    <dt><em>writePolicy</em></dt>
    <dd>configures the writability of the socket</dd>
    
    <dt><em>bufferingPolicy</em></dt>
    <dd>configures, if and how buffering is configured for a socket</dd>
    </dl>

    Every Policy value is identified by a class type. The policy
    classes themselves built an inheritance hierarchy for each policy
    axis. For each policy axis, the root of this tree is the class
    named \i Policy \c Base (e.g. \p AddressingPolicyBase).

    The senf::SocketPolicy defines the complete policy of a socket. It
    combines a set of policy classes, one for each policy
    axis. Together, they define the behavior of a socket handle. The
    socket handle instances do not implement any socket functionality
    themselves instead defering the implementation to the policy
    classes. The SocketHandle interface is therefore \e not
    implemented using virtual members, all important socket functions
    can be inlined by the compiler to create highly efficient code.

    A SocketPolicy can be incomplete. In this case it does \e not
    completely specify the socket interface, it leaves some aspects
    open. A SocketHandle based on such a policy will have a reduced
    interface: It will only support those members for wich the
    corresponding policies are defined.

    Two SocketHandle's with different policies can be \e
    compatible. If they are, the more derived SocketHandle can be
    converted (assigned to) the more basic SocketHandle.
    
    \section policy_group_details The Policy Framework Classes

    In the following discussion we will use the following conventions:
    \li \e Policy is one or \c AddressingPolicy, \c FramingPolicy, \c
	CommunicationPolicy, \c ReadPolicy, \c WritePolicy or \c
	BufferingPolicy
    \li \e socketPolicy is any socket policy (that is, an
	instantiation of the SocketPolicy template)
    \li \e trait is an any policy class (that is, any class derived
	from one of the axis base classes)

    Each axis is comprised of a number of classes and templates (all
    in namespace senf of course):
    
    <dl>
    <dt>\e Policy \c Base (ex: AddressingPolicyBase)</dt>
    <dd>Baseclass of all policies in this axis</dd>

    <dt>\c Unspecified \e Policy  (ex: \ref UnspecifiedAddressingPolicy)</dt>
    <dd>An alias (typedef) for \e Policy \c Base</dd>

    <dt>\e Policy \c Is < \e socketPolicy, \e trait > (ex: AddressingPolicyIs)</dt>
    <dd>A template metafunction returning \c boost::true_type, if \e
    trait (any class derived from \e Policy \c Base) is a compatible
    policy value of the given \e socketPolicy</dd>

    <dt>\c If \e Policy \c Is < \e socketPolicy, \e trait > (ex: IfAddressingPolicyIs)</dt>
    <dd>This is a combination of \e Policy \c Is and \c boost::enable_if</dd>

    <dt>\c If \e Policy \c IsNot < \e socketPolicy, \e trait > (ex: IfAddressingPolicyIsNot)</dt>
    <dd>The inverse of above</dd>
    </dl>
    
    These classes form the basis of the policy framework. To bind the
    policy axis together, there are some more classes and templates.

    <dl>
    <dt>\c class \c SocketPolicyBase</dt>
    <dd>This class is the base class of the SocketPolicy template. It
    is used to validate, that a class is really a SocketPolicy (by
    checking, that it derives from SocketPolicyBase. This is simpler
    than chacking the template directly).</dd>

    <dt>\c template \c SocketPolicy < \e addressingPolicy, \e framingPolicy, \e communicationPolicy, \e readPolicy, \e writePolicy, \e bufferingPolicy ></dt>
    <dd>This is the central SocketPolicy template. It combines a
    complete set of policy classes, one for each axis.</dd>

    <dt>\c template \c MakeSocketPolicy < \e args ></dt>
    <dd>\c MakeSocketPolicy is a template metafunction which
    simplifies building SocketPolicy instantiations. It takes any
    number (ok, up to a maximum of 6) of policy classes as an
    argument (in any order). It will sort these arguments into the
    SocketPolicy template arguments. If for some axis no class is
    specified, it's slot will be filled with \c Unspecified \e
    Policy. Additionally, the first Argument may optionally be ab
    arbitrary SocketPolicy. It will provide default values for
    unspecified axis</dd>

    <dt>\c template \c SocketPolicyIsBaseOf < \e base, \e derived ></dt>
    <dd>This template metafunction will check, wether the socket
    policy \e derived is convertible to \e base. This means, that for
    each axis, the corresponding policy class in \e derived must be
    derived or be the same as the one on \e base.</dd>
    </dl>

    \implementation All these classes are created automatically. The
    \c SENF_SOCKET_POLICIES makro is a Boost.Preprocessor style
    sequence listing all policy axis. The Boost.Preprocessor library
    is then used to generate the respective classes.

    \section policy_implement Implementing Policy Classes

    To define a new policy class, derive from the corresponding base
    class for your policy axies. The only policy axis which might
    possibly need to be extended are the addressing policy
    (AddressingPolicyBase) and the buffering policy
    (BufferingPolicyBase). See the Documentation of these classes for
    more information on which members can be implemented.

    All members you define must be static. For any of the policy
    classes, you must only define those members which are supported by
    your implementation. If you leave out a member you automatically
    disable the corresponding functionality in the
    ClientSocketHandle/ServerSocketHandle interface.

    The member prototypes given in the base class documentation only
    specify the call signature not the way, the member must be defined
    (FileHandle really is not a FileHandle but an arbitrary
    SocketHandle).
    
    If the existence of a member depends on other policies, you should
    use the <code>If</code><i>SomePolicy</i><code>Is</code> and
    <code>If</code><i>SomePolicy</i><code>IsNot</code> templates to
    dynamically enable/disable the member depending on some other
    policy:

    \code
      struct ExampleAddressingPolicy 
      {
	  template <class Policy>
	  void connect(senf::SocketHandle<Policy> handle, Address & addr,
		       typename senf::IfCommmunicationPolicyIs<
			   Policy, senf::ConnectedCommunicationPolicy>::type * = 0);
      };
    \endcode

    The \c connect member in this example will only be enabled, it
    the communication policy of the socket handle is
    ConnectedCommunicationPolicy (or a derived type). See <a
    href="http://www.boost.org/libs/utility/enable_if.html">Boost.Enable_If</a>
    for a discussion of the third argument (\c
    senf::ConnectedCommunicationPolicyIs is based on the \c
    boost::enable_if template).

    \see \ref extend_policy \n
         <a class="ext" href="http://www.boost.org/libs/utility/enable_if.html">The Boost enable_if utility</a> \n
         <a class="ext" href="http://www.boost.org/libs/mpl/doc/index.html">The Boost.MPL library</a> \n
         <a class="ext" href="http://www.boost.org/libs/preprocessor/doc/index.html">The Boost.Preprocessor library</a>

    \idea We could combine all the \e Policy \c Is templates into a
    single template. Since the \e trait argument will automatically
    specify the axis to be used, it is not necessary to specify that
    axis in the tempalte functor's name. We could even combine this
    with \c SocketPolicyIsBaseOf.
 */

#ifndef HH_SocketPolicy_
#define HH_SocketPolicy_ 1

// Custom includes

#include "GenericSockAddr.hh"

//#include "SocketPolicy.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup policy_group
    /// @{

    // This may be adapted to change the supported policies (however,
    // ClientSocketHandle and ServerSocketHandle will probably have to
    // be adjusted accordingly)

    /** \brief List all policy axis
	
	\internal

	This define symbol is used to configure the policy axis. The
	base class for each of these axis must be defined explicitly
	(e.g. AddressingPolicyBase). The implementation files will
	then automatically generate all the other classes from this
	list.

	\see policy_group
     */
#   define SENF_SOCKET_POLICIES			\
	(AddressingPolicy)			\
	(FramingPolicy)				\
	(CommunicationPolicy)			\
	(ReadPolicy)				\
	(WritePolicy)				\
	(BufferingPolicy)

    // Wer define these classes explicitly (and not with some macro
    // magic) because
    // a) AddressingPolicyBase is different from all the others
    // b) We want to document each one explicitly
    
    /** \brief Policy defining socket addressing

	AddressingPolicyBase is the baseclass of all addressing policy
	classes. When defining a new addressing policy, the following
	members can be defined. All methods must be static.

	<table class="senf">
	<tr><td>typedef</td> <td><tt>Address</tt></td>					 <td>Address type</td></tr>
	<tr><td>method</td>  <td><tt>void local(FileHandle, Address &)</tt></td>	 <td>Get local socket address</td></tr>
	<tr><td>method</td>  <td><tt>void peer(FileHandle, Address &)</tt></td>		 <td>Get remote socket address</td></tr>
	<tr><td>method</td>  <td><tt>void bind(FileHandle, Address const &)</tt></td>	 <td>Bind socket to local address</td></tr>
	<tr><td>method</tr>  <td><tt>void connect(FileHandle, Address const &)</tt></td> <td>Connect to remote address</td></tr>
	</table>

	\see policy_group
     */
    struct AddressingPolicyBase
    {
	virtual ~AddressingPolicyBase() {}
	
	typedef GenericSockAddr Address;
    };

    /** \brief Policy defining the framing format

	This policy does not define any operations since it does have
	no influence on any method signature. It does however affect
	the semantics of the \c read() and \c write() operations.

	\note This policy axis probably only has two sensible statess:
	StreamFramingPolicy and DatagramFramingPolicy.

	\see policy_group
     */
    struct FramingPolicyBase 
    {
	virtual ~FramingPolicyBase() {}
    };

    /** \brief Policy defining, how peers are selected

	The CommunicationPolicy may define two members:

	<table class="senf">
	<tr><td>method</td> <td><tt>void listen(FileHandle, unsigned backlog)</tt></td> <td>Switch socket into listening state</td></tr>
	<tr><td>method</td> <td><tt>int accept(FileHandle, Address &)</tt></td>		<td>Accept a new connection</td></tr>
	</table>
	
	The \c listen member is straight forward. The \c accept() member
	must return a new file descriptor (which will be used to
	create a new SocketHandle of the correct type). Additionally,
	accept() should only be defined, if the Addressing policy is
	not \c NoAddressingPolicy (which together with
	ConnectedCommunicationPolicy would identify a point-to-point
	link with fixed communication partners).

	\note This Policy only has two meaningful states:
	ConnectedCommunicationPolicy and
	UnconnectedCommunicationPolicy. It is probably not sensible to
	define a new CommunicationPolicy type.

	\see policy_group
     */
    struct CommunicationPolicyBase
    {
	virtual ~CommunicationPolicyBase() {}
    };

    /** \brief Policy defining the readability

	The ReadPolicy defines, wether the socket is readable. It
	may define two members:

	<table class="senf">
	<tr><td>method</td> <td><tt>unsigned read(FileHandle, char * buffer, unsigned size)</tt></td>		     <td>read data from socket</td></tr>
	<tr><td>method</td> <td><tt>unsigned readfrom(FileHandle, char * buffer, unsigned size, Address &)</tt></td> <td>read data from unconnected socket</td></tr>
	</table>

	The second member should only be enabled if the communication
	policy is UnconnectedCommunication (otherwise it does not make
	sense since the communication partner is fixed) (see
	AddressingPolicyBase on how to do this).

	\note This Policy only has two meaningful states:
	ReadablePolicy and NotReadablePolicy. It probably does not
	make sense to define new read policy types.

	\see policy_group
     */
    struct ReadPolicyBase
    {
	virtual ~ReadPolicyBase() {}
    };

    /** \brief Policy defining the writability

	The WritePolicy defines, wether the socket is writable. It may
	define two members:

	<table class="senf">
	<tr><td>method</td> <td><tt>unsigned write(FileHandle, char * buffer, unsigned size)</tt></td>		    <td>read data from socket</td></tr>
	<tr><td>method</td> <td><tt>unsigned writeto(FileHandle, char * buffer, unsigned size, Address &)</tt></td> <td>read data from unconnected socket</td></tr>
	</table>

	The second member should only be enabled if the communication
	policy is UnconnectedCommunication (otherwise it does not make
	sense since the communication partner is fixed) (see
	AddressingPolicyBase on how to do this).

	\note This Policy only has two meaningful states:
	WritablePolicy and NotWritablePolicy. It probably does not
	make sense to define new write policy types.

	\see policy_group
     */
    struct WritePolicyBase
    {
	virtual ~WritePolicyBase() {}
    };
    
    /** \brief Policy defining the buffering interface

	The BufferingPolicy defines the buffer handling of the
	socket. It may provide the follogin members:
	
	\see policy_group
     */
    struct BufferingPolicyBase
    {
	virtual ~BufferingPolicyBase() {}
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
	
	This template is an example of the \i Policy \c Is family of
	tempalte metafunctions. It will check, wether \c Trait is a
	valid compatible Policy class of \c SocketPolicy. \c Trait
	must be derived from AddressingPolicyBase (respectively \i
	Policy \c Base). 

	\see \ref policy_group
     */
    template <class SocketPolicy, class Trait>
    struct AddressingPolicyIs
    {};

    /** \brief Enable template overload depending on policy value

	This template is an exmaple of the \c If \i Policy \c Is
	family of templates. It is used like <a class="ext"
	href="http://www.boost.org/libs/utility/enable_if.html">Boost.enable_if</a>
	to enable a templated overload only, if the AddressingPolicy
	of \i Policy is compatible with \c Trait (that is the
	AddressingPolicy of \c Policy is derived from \c Trait).

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

	This class is used to 

	\see policy_group
     */
    struct SocketPolicyBase
    {};

    /** \brief Collection of policy classes
	
	The SocketPolicy template defines the complete Policy used by
	the socket library. It contains one policy class for each
	policy axis.

	A SocketPolicy can be complete or incomplete. An incomplete
	SocketPolicy will have at least one axis set to \c Undefined
	\i Policy (or a generic derived class which is used to group
	some other policies but does not (completely) define the
	policy behavior). A complete SocketPolicy will have a
	concrete definition of the desired behavior for each policy
	axis.

	\see policy_group
     */
    template <
	class AddressingPolicy, 
	class FramingPolicy,
	class CommunicationPolicy, 
	class ReadPolicy,
	class WritePolicy,
	class BufferingPolicy >
    struct SocketPolicy
    {};
    
    /** \brief Metafunction to create SocketPolicy
	
	This template metafunction simplifies the creation of a
	SocketPolicy instantiation. It takes any number (that is up to
	6) of Policy classes as arguments in any Order. It will create
	a SocketPolicy from these policy classes. Any axis not
	specified will be left as \c Unspecified \i Policy. 

	\see policy_group
     */
    template <class Arg1, class Arg2, class ArgN>
    struct MakeSocketPolicy
    {};

    /** \brief Check policy compatibility

	This tempalte metafunction checks, wether the SocketPolicy \c
	Derived is more specialized than \c Base (and therefore a
	SocketHandle with policy \c Derived is convertible to a
	SocketHandle with policy \c Base).

	The metafunction will return true (that is inherits from \c
	boost::true_type, see the <a class="ext"
	href="http://www.boost.org/libs/mpl/doc/index.html">Boost.MPL</a>
	library documentation for more information) if each policy
	class in \c Base is a baseclass of (or the same as) the
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
// c-file-style: "senf"
// End:
