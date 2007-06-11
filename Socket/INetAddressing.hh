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
    \brief INet[46]Address and INet[46]AddressingPolicy public header
 */

#ifndef HH_INetAddressing_
#define HH_INetAddressing_ 1

// Custom includes
#include <string>
#include <exception>
#include <netinet/in.h>
#include "SocketPolicy.hh"
#include "ClientSocketHandle.hh"
#include "CommunicationPolicy.hh"
#include "GenericAddressingPolicy.hh"

//#include "INetAddressing.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup addr_group
    /// @{

    /** \brief IPv4 socket address

        INet4Address wraps the standard sockaddr_in datatype. It provides simple accessor methods
        to access the host and port. It does \e not integrate \c gethostbyname or DNS lookup.

        \todo Implement real INet4Address datatype and rename this one to INet4SockAddress ...
        \todo Implement more complete interface
        \todo  gethostbyname support ?
     */
    class INet4Address
    {
    public:
        INet4Address();
        INet4Address(char const * address); ///< Set address and port
                                        /**< See INet4Address(std::string)
                                             \throws InvalidINetAddressException */
        INet4Address(std::string address); ///< Set address and port
                                        /**< This constructor expects a string of the form
                                             'xxx.xxx.xxx.xxx:pppp'. The constructor will use this
                                             value to initialize the host and port members. This
                                             constructor does \e only support numeric ip addresses
                                             not hostnames
                                             \param[in] address Address and port
                                             \throws InvalidINetAddressException */
        INet4Address(std::string host, unsigned port); ///< Set address and port explicitly
                                        /**< \param[in] host ip address in dotted-quad notation
                                             \param[in] port port number
                                             \throws InvalidINetAddressException */


        bool operator==(INet4Address const & other) const; ///< Check INet4Address for equality

        std::string str() const;        ///< Return "address:port" string
        std::string host() const;       ///< Return address in doted quad notation
        unsigned port() const;          ///< Return portnumber

        void clear();                   ///< Clear address/port to 0.0.0.0:0

        /// \name Generic Address Interface
        /// @{

        struct sockaddr * sockaddr_p();
        struct sockaddr const * sockaddr_p() const;
        unsigned sockaddr_len() const;

        /// @}

    private:
        void assignString(std::string addr);

        struct ::sockaddr_in addr_;
    };

    /** \brief Write address and port to os

        \related INet4Address
     */
    std::ostream & operator<<(std::ostream & os, INet4Address const & addr);

    /** \brief IPv6 network address

        INet6Address represents a 128bit IPv6 network address. This class supports all standard
        numeric string representations of IPv6 addresses. This class does not integrate with \c
        gethostbyname() and so does not support host names.

        The conversion constructors allow the use of string constants wherever an INet6Address is
        expected. Especially, it is possible to assign a string to an address to change it's value.

        \implementation The <tt>char const *</tt> constructor overload is needed to support
            string-literals where an INet6Address is expected (the C++ standard does not allow
            chaining conversion constructors like char const * -> std::string -> INet6Address)
     */
    class INet6Address
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        INet6Address();                 ///< Create empty address
        INet6Address(std::string const & addr); ///< Create address from string representation
        INet6Address(char const * addr); ///< Create address from string representation
        INet6Address(struct in6_addr const & addr); ///< Create address from in6_addr
        template <class Range>
        explicit INet6Address(Range const & range); ///< Create address from arbitrary raw data
                                        /**< This constructor will copy 16 bytes from the given
                                             range and interpret them as a IPv6 address in network
                                             byte order. This constructor is used to read an
                                             arbitrary address from it's binary representation.

                                             \param range arbitrary range, see <a
                                                 href="http://www.boost.org/libs/range/index.html">Boost.Range</a>
                                          */

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        void clear();                   ///< Clear address
        std::string address() const;    ///< Return printable address representation

        bool operator==(INet6Address const & other) const; ///< Compare addresses for equality
        bool operator!=(INet6Address const & other) const; ///< Inverse of above

        struct in6_addr & addr();       ///< Access internal address representation
        struct in6_addr const & addr() const;
                                        ///< Access internal address representation in const context
        struct in6_addr * addr_p();     ///< Get pointer to internal address repr
        struct in6_addr const * addr_p() const;
                                        ///< Get const pointer to internal address repr
        unsigned addr_len() const;      ///< Size of an IPv6 address (16 bytes)

    protected:

    private:
        struct in6_addr addr_;
    };


    /** \brief Output INet6Address instance as it's string representation
     */
    std::ostream & operator<<(std::ostream & os, INet6Address const & addr);

    /** \brief IPv6 socket address

        This class wraps the standard \c sockaddr_in6 structure. INet6SocketAddress provides access
        to all members of the sockaddr_in6 structure. Additionally, INet6SocketAddress supports the
        string representation

        \par "" <tt>[</tt> <i>address</i> [ <tt>\@</tt> <i>interface</i> ] <tt>]:</tt> <i>port</i>

        Where \e address is an arbitrary numeric IPv6 address, \e interface is an optional network
        interface name and \e port is the port number. The interface specification is only valid if
        \e address is link-local address. The URL representation of an IPv6 address is as above
        without the optional interface spec.

        INet6SocketAddress supports conversion constructors from it's string
        representation. Therefore, wherever a INet6SocketAddress instance is expected, a string may
        be used instead.

        \implementation The sockaddr_in6 structure has an sin6_flowinfo member. However RFC3493 does
            not give the use of this field and specifies, that the field should be ignored ... so
            that's what we do. Furthermore, the GNU libc reference states, that this field is not
            implemented in the library.

        \implementation We need to return the address in host() by value since we need to return a
            INet6Address. However, sockaddr_in6 does not have one ...

        \implementation The <tt>char const *</tt> constructor overload is needed to support
            string-literals where an INet6SocketAddress is expected (the C++ standard does not allow
            chaining conversion constructors like <tt>char const *</tt> -> \c std::string -> \c
            INet6SocketAddress)

        \idea Implement a INet6Address_ref class which has an interface identical to INet6Address
        and is convertible to INet6Address (the latter has a conversion constructor taking the
        former as arg). This class however references an external in6_addr instead of containing one
        itself. This can be used in INet6SocketAddress to increase the performance of some
        operations.
     */
    class INet6SocketAddress
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        INet6SocketAddress();           ///< Create empty instance
        INet6SocketAddress(std::string const & addr);
                                        ///< Initialize/convert from string representation
        INet6SocketAddress(char const * addr); ///< Same as above to support string literals
        INet6SocketAddress(INet6Address const & addr, unsigned port);
                                        ///< Initialize from address and port
        INet6SocketAddress(INet6Address const & addr, unsigned port, std::string const & iface);
                                        ///< Initialize explicitly from given parameters
        INet6SocketAddress(std::string const & addr, std::string const & iface);
                                        ///< Initialize from URL representation and explit interface

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        bool operator==(INet6SocketAddress const & other) const; ///< Check addresses for equality
        bool operator!=(INet6SocketAddress const & other) const; ///< Inverse of above

        void clear();                   ///< Clear socket address

        std::string address() const;    ///< Get printable address representation

        INet6Address host() const;      ///< Get address
        void host(INet6Address const & addr); ///< Change address

        unsigned port() const;          ///< Get port number
        void port(unsigned poirt);      ///< Change port number

        std::string iface() const;      ///< Get interface name
        void iface(std::string const & iface); ///< Change interface

        ///\name Generic SocketAddress interface
        ///@{

        struct sockaddr * sockaddr_p();
        struct sockaddr const * sockaddr_p() const;
        unsigned sockaddr_len() const;

        ///@}

    protected:

    private:
        void assignAddr(std::string const & addr);
        void assignIface(std::string const & iface);

        struct sockaddr_in6 sockaddr_;
    };

    /** \brief Output INet6SocketAddress instance as it's string representation
     */
    std::ostream & operator<<(std::ostream & os, INet6SocketAddress const & addr);

    /** \brief Signal invalid INet address syntax

        \related INet4Address
        \relatesalso INet6Address
     */
    struct InvalidINetAddressException : public std::exception
    { char const * what() const throw() { return "invalid inet address"; } };

    /// @}

    /// \addtogroup policy_impl_group
    /// @{

    /** \brief Addressing policy supporting IPv4 addressing

        \par Address Type:
            INet4Address

        This addressing policy implements addressing using Internet V4
        addresses.

        The various members are directly imported from
        GenericAddressingPolicy which see for a detailed
        documentation.
     */
    struct INet4AddressingPolicy
        : public AddressingPolicyBase,
          private GenericAddressingPolicy<INet4Address>
    {
        typedef INet4Address Address;

        using GenericAddressingPolicy<INet4Address>::peer;
        using GenericAddressingPolicy<INet4Address>::local;
        using GenericAddressingPolicy<INet4Address>::connect;
        using GenericAddressingPolicy<INet4Address>::bind;
    };

    /** \brief Addressing policy supporting IPv6 addressing

        \par Address Type:
            INet6SocketAddress

        This addressing policy implements addressing using Internet V6
        addresses.

        The various members are directly imported from
        GenericAddressingPolicy which see for a detailed
        documentation.
     */
    struct INet6AddressingPolicy
        : public AddressingPolicyBase,
          private GenericAddressingPolicy<INet6SocketAddress>
    {
        typedef INet6SocketAddress Address;

        using GenericAddressingPolicy<INet6SocketAddress>::peer;
        using GenericAddressingPolicy<INet6SocketAddress>::local;
        using GenericAddressingPolicy<INet6SocketAddress>::connect;
        using GenericAddressingPolicy<INet6SocketAddress>::bind;
    };

    /// @}

}

///////////////////////////////hh.e////////////////////////////////////////
#include "INetAddressing.cci"
#include "INetAddressing.ct"
//#include "INetAddressing.cti"
//#include "INetAddressing.mpp"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
