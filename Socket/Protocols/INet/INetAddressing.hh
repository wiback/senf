// $Id$
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
    \brief INet[46]Address and INet[46]AddressingPolicy public header
 */

#ifndef HH_SENF_Socket_Protocols_INet_INetAddressing_
#define HH_SENF_Socket_Protocols_INet_INetAddressing_ 1

// Custom includes
#include <string>
#include <exception>
#include <netinet/in.h>
#include "../../SocketPolicy.hh"
#include "../../ClientSocketHandle.hh"
#include "../../CommunicationPolicy.hh"
#include "../BSDAddressingPolicy.hh"
#include "../BSDSocketAddress.hh"
#include "INet4Address.hh"
#include "INet6Address.hh"

//#include "INetAddressing.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief IPv4 socket address

        INet4Address wraps the standard sockaddr_in datatype. It provides simple accessor methods
        to access the host and port. It does \e not integrate \c gethostbyname or DNS lookup.

        \implementation This implementation is based on sockaddr_in, which is needed since it needs
            to provide a non-const struct sockaddr * for legacy compatibility.

        \ingroup addr_group
     */
    class INet4SocketAddress
        : public BSDSocketAddress
    {
    public:
        static short const addressFamily = AF_INET;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        INet4SocketAddress();
        explicit INet4SocketAddress(std::string const & addr); ///< Set address and port
                                        /**< This constructor expects a string of the form
                                             'host:port'. The constructor will use this value to
                                             initialize the host and port members. Since it uses the
                                             INet4Address::from_string constructor, this call may
                                             block while waiting for the resolver.
                                             \throws AddressSyntaxException if the address syntax is
                                                 invalid
                                             \throws UnknownHostnameException if the
                                                 address cannot be resolved. */

        INet4SocketAddress(INet4Address const & addr, unsigned port);
                                        ///< Set address and port explicitly
                                        /**< \param[in] addr IP address
                                             \param[in] port port number */

        explicit INet4SocketAddress(unsigned port);
                                        ///< Set port, address is set to 0.0.0.0
                                        /**< \param[in] port port number */

        INet4SocketAddress(const INet4SocketAddress& other);
        INet4SocketAddress& operator=(const INet4SocketAddress& other);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        INet4Address address() const;   ///< Return address
        unsigned port() const;          ///< Return port number

        void address(INet4Address const & addr); ///< Set address
        void port(unsigned p);          ///< Set port number

        using BSDSocketAddress::sockaddr_p;
        using BSDSocketAddress::socklen_p;

    private:
        struct ::sockaddr_in addr_;
    };

    /** \brief Write address and port to os

        \related INet4SocketAddress
     */
    std::ostream & operator<<(std::ostream & os, INet4SocketAddress const & addr);
    std::istream & operator>>(std::istream & is, INet4SocketAddress & addr);

    /** \brief IPv6 socket address

        This class wraps the standard \c sockaddr_in6 structure. INet6SocketAddress provides access
        to all members of the sockaddr_in6 structure. Additionally, INet6SocketAddress supports the
        string representations

        \par ""
            <tt>[</tt> <i>address</i> [ <tt>%</tt> <i>zone-id</i> ] <tt>]:</tt> <i>port</i> \n
            <i>hostname</i> <tt>:</tt> <i>port</i>

        Where \e address is an arbitrary numeric IPv6 address, \e zone-id is an optional network
        interface name and \e port is the port number. So some example addresses are

        \par ""
            <tt>[2001:db8:1::1]:80</tt> \n
            <tt>www.go6.net:80</tt> \n
            <tt>[fe80::1\%eth0]:443</tt>

        \implementation The sockaddr_in6 structure has an sin6_flowinfo member. However RFC3493 does
            not give the use of this field and specifies, that the field should be ignored ... so
            that's what we do. Furthermore, the GNU libc reference states, that this field is not
            implemented in the library.

        \idea Implement a INet6Address_ref class which has an interface identical to INet6Address
            and is convertible to INet6Address (the latter has a conversion constructor taking the
            former as arg). This class however references an external in6_addr instead of containing
            one itself. This can be used in INet6SocketAddress to increase the performance of some
            operations.

        \ingroup addr_group
     */
    class INet6SocketAddress
        : public BSDSocketAddress
    {
    public:
        static short const addressFamily = AF_INET6;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        INet6SocketAddress();           ///< Create empty instance
        explicit INet6SocketAddress(std::string const & addr,
                                    INet6Address::Resolve_t resolve = INet6Address::ResolveINet6);
                                        ///< Initialize/convert from string representation
                                        /**< \throws AddressSyntaxException if the address syntax is
                                                 invalid
                                             \throws UnknownHostnameException if the
                                                 address cannot be resolved.
                                             \param[in] addr Address to parse
                                             \param[in] resolve If this is
                                                 INet6Address::ResolveINet4, support IPv4
                                                 addresses. See INet6Address. */
        INet6SocketAddress(INet6Address const & addr, unsigned port);
                                        ///< Initialize from address and port
        INet6SocketAddress(INet6Address const & addr, unsigned port, std::string const & iface);
                                        ///< Initialize explicitly from given parameters
                                        /**< \throws AddressSyntaxException if the
                                                given iface cannot be resolved. */
        explicit INet6SocketAddress(unsigned port);
                                        ///< Initialize from port and set to 'unspecified' addr
                                        /**< The address is set to [::]
                                             \param[in] port port number  */

        INet6SocketAddress(const INet6SocketAddress& other);
        INet6SocketAddress& operator=(const INet6SocketAddress& other);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        INet6Address address() const;    ///< Get printable address representation
        void address(INet6Address const & addr); ///< Change address

        unsigned port() const;          ///< Get port number
        void port(unsigned port);       ///< Change port number

        std::string iface() const;      ///< Get interface name
        void iface(std::string const & iface); ///< Change interface
                                               /**< \throws AddressSyntaxException if the
                                                        given iface cannot be resolved. */

        using BSDSocketAddress::sockaddr_p;
        using BSDSocketAddress::socklen_p;

    protected:

    private:
        void assignIface(std::string const & iface);

        struct sockaddr_in6 sockaddr_;
    };

    /** \brief Output INet6SocketAddress instance as it's string representation
        \related INet6SocketAddress
     */
    std::ostream & operator<<(std::ostream & os, INet6SocketAddress const & addr);
    std::istream & operator>>(std::istream & is, INet6SocketAddress & addr);

    /// \addtogroup policy_impl_group
    /// @{

    /** \brief Addressing policy supporting IPv4 addressing

        \par Address Type:
            INet4SocketAddress

        This addressing policy implements addressing using Internet V4
        addresses.

        The various members are directly imported from
        BSDAddressingPolicyMixin which see for a detailed
        documentation.
     */
    struct INet4AddressingPolicy
        : public BSDAddressingPolicy,
          private BSDAddressingPolicyMixin<INet4SocketAddress>
    {
        typedef INet4SocketAddress Address;

        using BSDAddressingPolicyMixin<INet4SocketAddress>::peer;
        using BSDAddressingPolicyMixin<INet4SocketAddress>::local;
        using BSDAddressingPolicyMixin<INet4SocketAddress>::connect;
        using BSDAddressingPolicyMixin<INet4SocketAddress>::bind;
    };

    /** \brief Addressing policy supporting IPv6 addressing

        \par Address Type:
            INet6SocketAddress

        This addressing policy implements addressing using Internet V6
        addresses.

        The various members are directly imported from
        BSDAddressingPolicyMixin which see for a detailed
        documentation.
     */
    struct INet6AddressingPolicy
        : public BSDAddressingPolicy,
          private BSDAddressingPolicyMixin<INet6SocketAddress>
    {
        typedef INet6SocketAddress Address;

        using BSDAddressingPolicyMixin<INet6SocketAddress>::peer;
        using BSDAddressingPolicyMixin<INet6SocketAddress>::local;
        using BSDAddressingPolicyMixin<INet6SocketAddress>::connect;
        using BSDAddressingPolicyMixin<INet6SocketAddress>::bind;
    };

    /// @}

}

///////////////////////////////hh.e////////////////////////////////////////
#include "INetAddressing.cci"
//#include "INetAddressing.ct"
//#include "INetAddressing.cti"
//#include "INetAddressing.mpp"
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
