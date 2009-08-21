// $Id$
//
// Copyright (C) 2007
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
    \brief INet4Address public header */

#ifndef HH_SENF_Socket_Protocols_INet_INet4Address_
#define HH_SENF_Socket_Protocols_INet_INet4Address_ 1

// Custom includes
#include <iostream>
#include <string>
#include <boost/cstdint.hpp>
#include <boost/array.hpp>
#include <boost/operators.hpp>
#include <senf/Utils/safe_bool.hh>
#include <senf/Utils/Tags.hh>
#include <senf/Socket/Protocols/AddressExceptions.hh>

//#include "INet4Address.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief IPv4 Internet address

        INet4Address represents a simple IP address. It is modelled as a fixed-size
        container/sequence of 4 bytes.

        The following statements all create the same INet4 address <code>211.194.177.160</code>
        \code
        // Used to construct constant INet4 addresses
        INet4Address(0xD3C2B1A0)

        // Construct an INet4 address from it's string representation. All the standard address
        // representations are supported
        INet4Address::from_string("211.194.177.160")
        INet4Address::from_string("211.12759456")

        // Construct an INet4 address from raw data. 'from_data' takes an arbitrary iterator (e.g. a
        // pointer) as argument. Here we use a fixed array but normally you will need this to build
        // an INet4 address in a packet parser
        char rawBytes[] = { 0xD3, 0xC2, 0xB1, 0xA0 };
        INet4Address::from_data(rawBytes)

        // Construct an INet4 address from the standard POSIX representation: a 32-bit integer in
        // network byte oder. This is used to interface with POSIX routines
        struct sockaddr_in saddr = ...;
        INet4Address::from_inaddr(saddr.sin_addr.s_addr)
        \endcode

        Since INet4Address is based on \c boost::array, you can access the raw data bytes of the
        address (in network byte order) using \c begin(), \c end() or \c operator[]
        \code
        INet4Address ina = ...;
        Packet::iterator i = ...;
        std::copy(ina.begin(), ina.end(), i); // Copies 4 bytes
        \endcode

        \see CheckINet4Network \n INet4Network

        \implementation We awkwardly need to use static named constructors (<tt>from_</tt> members)
            instead of ordinarily overloaded constructors for one simple reason: <tt>char *</tt>
            doubles as string literal and as arbitrary data iterator. The iterator constructor can
            therefore not be distinguished from initialization with a string literal. Therefore we
            need to disambiguate using the named constructors.

        \ingroup addr_group
      */
    class INet4Address
        : public boost::array<boost::uint8_t,4>,
          public comparable_safe_bool<INet4Address>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef uint32_t address_type;  ///< Address representation as number in host byte order
        typedef uint32_t inaddr_type;   ///< Legacy address representation in network byte order

        static INet4Address const None; ///< The empty (0) address
        static INet4Address const Loopback; ///< The loopback (127.0.0.1) address
        static INet4Address const Broadcast; ////< The global broadcast (255.255.255.255) address

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        INet4Address();                 ///< Construct an empty address
        explicit INet4Address(senf::NoInit_t); ///< Construct uninitialized (!) address
        explicit INet4Address(address_type value);
                                        ///< Construct an address constant

        static INet4Address from_string(std::string const & s);
                                        ///< Convert string to address
                                        /**< This member will try to convert the given string into
                                             an IP address. from_string() supports all standard IP
                                             literal representations as well as hostnames.
                                             \attention This call may block if \a s represents a
                                                 hostname which must be looked up via some network
                                                 protocol like DNS or NIS
                                             \throws AddressSyntaxException if the address cannot be
                                                 converted for some reason
                                             \param[in] s Address literal or hostname */

        template <class InputIterator>
        static INet4Address from_data(InputIterator i);
                                        ///< Construct address from 4 bytes of raw data
                                        /**< from_data will build an address from 4 bytes of raw
                                             data as accessed by the iterator. The data must be in
                                             network byte order. */
        static INet4Address from_inaddr(inaddr_type v);
                                        ///< Construct address from integer in network byte order
                                        /**< This call is used when interfacing with other legacy
                                             code to convert a network byte order address in an
                                             integer number into an INet4Address. */

        ///@}
        ///////////////////////////////////////////////////////////////////////////
        ///\name Accessors
        ///@{

        bool local() const;             ///< \c true, if address is locally administered
                                        /**< This call checks, if the address is within one of the
                                             IANA private ranges. */
        bool loopback() const;          ///< \c true, if address is within the loopback network
                                        /**< Checks, whether the address is in the IANA loopback
                                             network 10.0.0.0/8 */
        bool multicast() const;         ///< \c true, if address is a multicast address
                                        /**< Checks, whether the address is in the 224.0.0.0/4
                                             network reserved for multicast addresses by the
                                             IANA. */
        bool broadcast() const;         ///< \c true, if address is 255.255.255.255
        bool boolean_test() const;      ///< \c true, if address is non-empty (!= 0.0.0.0)

        inaddr_type inaddr() const;     ///< Return the raw network byte order address
                                        /**< This member is used to interact with legacy code.
                                             \return */
        address_type address() const;   ///< Return address represented as integer number
                                        /**< This member returns the address as an integer number in
                                             host byte order. This representation allows simple
                                             network math operations. */
        ////@}

    private:
        enum InAddr_t { IsInAddr };
        INet4Address(inaddr_type addr, InAddr_t);
        inaddr_type & iref();
        inaddr_type iref() const;
    };

    /** \brief Output INet4Address instance as it's string representation
        \related INet4Address
     */
    std::ostream & operator<<(std::ostream & os, INet4Address const & addr);

    /** \brief Initialize INet4Address instance from a string representation
        sets std::ios::failbit on the stream if an error occurred
        \see INet4Address from_string()
        \related INet4Address
     */
    std::istream & operator>>(std::istream & os, INet4Address & addr);

    /** \brief CHeck INet4Address against a fixed network prefix

        This helper allows to easily and efficiently check an INet4Address against an arbitrary but
        constant network prefix. The network prefix is represented by

        \par ""
            <tt>senf::CheckINet4Network<</tt> <i>addr</i> <tt>,</tt> <i>prefix-len</i> <tt>></tt>

        Where \a addr is the v4 Internet address as a 32-bit unsigned integer number in host byte
        order and \a prefix_len is the length of the network prefix. The class exposes a single
        static member <tt>match(</tt> <i>addr</i> <tt>)</tt> which matches the INet4Address \a addr
        against the prefix:

        \code
        if (senf::CheckINet4Network<0x7F000000u,8u>::match(addr)) {
            // 'addr' is within the 127.0.0.0/8 loopback network
            ...
        }
        \endcode

        \implementation This is implemented the way it is so the syntax is identical to the
            CheckINet6Network syntax.
     */
    template <boost::uint32_t address, unsigned prefix_len>
    class CheckINet4Network
    {
    public:
        static bool match(INet4Address const & addr);
    };

    /** \brief IPv4 network prefix

        This class represents an IPv4 network prefix in CIDR notation.
      */
    class INet4Network
        : public boost::equality_comparable<INet4Network>,
          public comparable_safe_bool<INet4Network>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        INet4Network();                 ///< Construct empty (0.0.0.0/0) network
        INet4Network(INet4Address const & address, unsigned prefix_len);
                                        ///< Construct network from given address and prefix length
        explicit INet4Network(std::string const & s); ///< Construct network from CIDR notation

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        INet4Address const & address() const; ///< Get the networks address
        unsigned prefix_len() const;    ///< Get the networks prefix length

        bool boolean_test() const;      ///< \c true, if INet4Network is non-empty
        bool operator==(INet4Network const & other) const;
                                        ///< Compare to networks for equality

        bool match(INet4Address const & addr) const; ///< \c true, if the network includes \a addr
        bool match(INet4Network const & net) const; ///< \c true, if the network includes \a net
                                        /**< The is true, if \a net is sub-network (or the same as)
                                             \c this. */

        INet4Address host(boost::uint32_t number); ///< Return the host with the given number
                                        /**< Returns the host with the given number within the
                                             network. If the number is larger than the maximum
                                             host number in the network, it is truncated. So \c
                                             host(0) is the networks own address, \c host(1)
                                             customarily is the default router and \c host(-1) is
                                             the broadcast address. */

        INet4Network subnet(boost::uint32_t net, unsigned prefix_len);
                                        ///< Return the given subnet of \c this
                                        /**< The returned INet4Network will be a subnet of \c this
                                             with the given network number. The network number is
                                             comprised by the bits above \a prefix_len:
                                             \code
                                             INet4Network("192.168.0.0/16").subnet(111u,24u) == INet4Network("192.168.111.0/24")
                                             INet4Network("192.168.111.0/24").subnet(1u,28u) == INet4Network("192.168.111.16/28")
                                             \endcode
                                             \param[in] net network number
                                             \param[in] prefix_len length of subnet prefix */

    protected:

    private:
        boost::uint32_t mask() const;

        unsigned prefix_len_;
        INet4Address address_;
    };

    /** \brief Output INet4Network instance as it's string representation
        \related INet4Network
     */
    std::ostream & operator<<(std::ostream & os, INet4Network const & addr);

    /** \brief Initialize INet4Address instance from a string representation
        sets std::ios::failbit on the stream if an error occurred
        \see INet4Address from_string()
        \related INet4Network
     */
    std::istream & operator>>(std::istream & is, INet4Network & addr);

}

///////////////////////////////hh.e////////////////////////////////////////
#include "INet4Address.cci"
#include "INet4Address.ct"
#include "INet4Address.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
