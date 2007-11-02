// $Id$
//
// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief INet6Address public header */

#ifndef HH_INet6Address_
#define HH_INet6Address_ 1

// Custom includes
#include <iostream>
#include <string>
#include <boost/cstdint.hpp>
#include <boost/array.hpp>
#include <boost/operators.hpp>
#include "../../../Utils/safe_bool.hh"
#include "INet4Address.hh"

//#include "INet6Address.mpp"
#include "INet6Address.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief IpV6 network address

        This implementation of an IpV6 address is based strictly on 
        <a href="http://tools.ietf.org/html/rfc4291">RFC 4291</a>: Internet Protocol
        Version 6 (IPv6) Addressing Architecture. This class provides accessors to all the
        information fields defined in this document.

        The IpV6 addressing architecture however has several other components defined in other
        RFC's. These RFC's should be implemented in additional modules:

        \li <a href="http://tools.ietf.org/html/rfc4193">RFC 4193</a>:
            Unique Local Addresses (ULA). Defines the fc00::/7 prefix
        \li <a href="http://tools.ietf.org/html/rfc3306">RFC 3306</a>:
            Unicast-Prefix-based IPv6 Multicast Addresses. Defines the ff30::/12 prefix
        \li <a href="http://tools.ietf.org/html/rfc3956">RFC 3956</a>:
            Embedding the Rendezvous Point (RP) Address in an IPv6 Multicast
            Address. Defines the ff70::/12 prefix
        \li <a href="http://tools.ietf.org/html/rfc3056">RFC 3056</a>:
            Connection of IPv6 Domains via IPv4 Clouds. Defines 6to4 tunneling and the
            2002::/16 prefix
        \li <a href="http://tools.ietf.org/html/rfc3849">RFC 3849</a>:
            IPv6 Address Prefix Reserved for Documentation. Defines the 2001:db8::/32 prefix
        
        Here an overview of well-known prefixes:

        <table class="senf">
        <tr><th>Prefix</th>                  <th>Description</th>                        <th>Definition</th> <th>Note</th></tr>
        <tr><td><tt>::/96</tt></td>          <td>IpV4 compatible IpV6 address</td>       <td>RFC4291</td>    <td>deprecated</td></tr>
        <tr><td><tt>::ffff:0:0/96</tt></td>  <td>IpV6 mapped IpV4 address</td>           <td>RFC4291</td>    <td></td></tr>
        <tr><td><tt>2000::/3</tt></td>       <td>Global unicast addresses</td>           <td>RFC3587</td>    <td>only noted, not defined</td></tr>
        <tr><td><tt>2001:db8::/32</tt></td>  <td>Documentation-only prefix</td>          <td>RFC3849</td>    <td></td></tr>
        <tr><td><tt>2002::/16</tt></td>      <td>6to4 addressing</td>                    <td>RFC3056</td>    <td></td></tr>
        <tr><td><tt>fc00::/7</tt></td>       <td>ULA</td>                                <td>RFC4193</td>    <td></td></tr>
        <tr><td><tt>fe80::/64</tt></td>      <td>Link-local addresses</td>               <td>RFC4291</td>    <td></td></tr>
        <tr><td><tt>fec0::/10</tt></td>      <td>Site-local addresses </td>              <td>RFC4291</td>    <td>deprecated</td></tr>
        <tr><td><tt>ff00::/8</tt></td>       <td>Multicast</td>                          <td>RFC4291</td>    <td></td></tr>
        <tr><td><tt>ff00::/12</tt></td>      <td>Globally allocated multicast</td>       <td>RFC4291</td>    <td></td></tr>
        <tr><td><tt>ff10::/12</tt></td>      <td>Locally allocated multicast</td>        <td>RFC4291</td>    <td></td></tr>
        <tr><td><tt>ff30::/12</tt></td>      <td>Unicast prefic based multicast</td>     <td>RFC3306</td>    <td></td></tr>
        <tr><td><tt>ff70::/12</tt></td>      <td>Multicast address with embedded RP</td> <td>RFC3956</td>    <td></td></tr>
        </table>

        The INet6Address class is based on \c boost::array and is built as a fixed-size sequence of
        16 bytes.

        \see CheckINet6Network \n INet6Network
        \ingroup addr_group

        \implementation We awkwardly need to use static named constructors (<tt>from_</tt> members)
            instead of ordinarily overloaded constructors for one simple reason: <tt>char *</tt>
            doubles as string literal and as arbitrary data iterator. The iterator constructor can
            therefore not be distinguished from initialization with a string literal. Therefore we
            need to disambiguate using the named constructors.
     */
    class INet6Address
        : public boost::array<boost::uint8_t,16>,
          public comparable_safe_bool<INet6Address>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        static INet6Address const None;        ///< The empty (::0) address
        static INet6Address const Loopback;    ///< The loopback (::1) address
        static INet6Address const AllNodes;    ///< The 'all nodes' link-local multicast address
        static INet6Address const AllRouters;  ///< The 'all routers' link-local multicast address

        enum NoInit_t { noinit };
        enum Resolve_t { ResolveINet6, ResolveINet4 };

        /** \brief Possible scope values

            List of all possible scope values. This list includes all scope values defined for
            multicast addresses in <a href="http://tools.ietf.org/html/rfc4291">RFC 4291</a>. 
            The values \ref LinkScope, \ref SiteScope and \ref GlobalScope are also used with 
            unicast addresses.
         */
        enum ScopeId {
              InterfaceScope    =  1    /**< Interface only scope */
            , LinkScope         =  2    /**< Link-local scope */
            , AdminScope        =  4    /**< Administration defined local scope */
            , SiteScope         =  5    /**< Site-local scope */
            , OrganizationScope =  8    /**< Scope covering multiple sites of an organization */
            , GlobalScope       = 14    /**< Global Internet scope */

            , ReservedScope     =  0    /**< Reserved scope value */
            , UnassignedScope   =  6    /**< Unassigned scope, may be defined locally */
        }; 

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        explicit INet6Address(NoInit_t); ///< Construct uninitialized (!) address
        INet6Address(boost::uint16_t a0=0u, boost::uint16_t a1=0u, boost::uint16_t a2=0u,
                     boost::uint16_t a3=0u, boost::uint16_t a4=0u, boost::uint16_t a5=0u,
                     boost::uint16_t a6=0u, boost::uint16_t a7=0u);
                                        ///< Construct an address constant

        static INet6Address from_string(std::string const & s, Resolve_t resolve = ResolveINet6);
                                        ///< Convert string to address
                                        /**< This member will try to convert the given string into
                                             an IP address. from_string() supports all standard IP
                                             literal representations as well es hostnames.
                                             \attention This call may block if \a s represents a
                                                 hostname which must be looked up via some network
                                                 protocol like DNS or NIS
                                             \throws SyntaxException if the address cannot be
                                                 converted for some reason
                                             \param[in] s Address literal or hostname 
                                             \param[in] resolve If this is set to \c ResolveINet4,
                                                 the call will additionally try to interpret \a s as
                                                 an IpV4 address if no valid IpV6 address is
                                                 found. The address will be returned as mapped IpV6
                                                 address. */

        template <class InputIterator> 
        static INet6Address from_data(InputIterator i);
                                        ///< Construct address from 16 bytes of raw data
                                        /**< from_data will build an address from 16 bytes of raw
                                             data as accessed by the iterator. The data must be in
                                             network byte order. */

        static INet6Address from_inet4address(INet4Address addr);
                                        ///< Construct an IpV6-mapped IpV4 address
                                        /**< This will construct an address of the form
                                             <tt>::FFFF::w.x.y.z</tt> where <tt>w.x.y.z</tt> is
                                             the INet4Address value. This kind of address is called
                                             an IpV6-mapped IpV4 address (see 
                                             <a href="http://tools.ietf.org/html/rfc4291">RFC 4291</a>).
                                             \par 
                                             IpV4 compatible IpV6 addresses are not directly
                                             supported, they are deprecated in the RFC. */
        ///@}
        ///////////////////////////////////////////////////////////////////////////
        ///\name Accessors
        ///@{

        boost::uint64_t network() const; ///< Return 64bit network part
        bool hasEuid64() const;         ///< \c true, if address is based on an EUID-64
        boost::uint64_t id() const;     ///< Return interface id (EUID-64)
        bool universalId() const;       ///< \c true, if the id() is universally assigned
        bool groupId() const;           ///< \c true, if the id()'s \a group bit is set

        bool unicast() const;           ///< \c true, if address is unicast
        bool multicast() const;         ///< \c true, if address is multicast

        ScopeId scope() const;          ///< Get address's scope
                                        /**< The scope of an address is one of the \ref ScopeId
                                             values. We need to differentiate between unicast and
                                             multicast addresses: unicast addresses only have local,
                                             site or global scope (where site scope is deprecated),
                                             multicast address can have a number of scope values of
                                             which local, site and global are a few. See the \ref
                                             ScopeId enumerators. */ 
        bool globalScope() const;       ///< \c true, if address is global unicast or multicast
        bool linkScope() const;         ///< \c true, if address is link-local unicast or multicast

        INet4Address inet4address() const; ///< Return embedded IpV4 address
                                        /**< Returns the IpV4 address embedded within an IpV4
                                             compatible or IpV4 mapped unicast address. This address
                                             is given by the last 32 bits of the IpV6 address. \par
                                             The value returned is only a valid IpV4 address if
                                             either ipv4Compatible() or ipv4Mapped() return \c
                                             true. */ 
        bool ipv4Compatible() const;    ///< \c true, if address is IpV4 compatible
                                        /**< IpV4 compatible IpV6 addresses are deprecated. */
        bool ipv4Mapped() const;        ///< \c true, if address is IpV4 mapped

        bool globalMulticastAddr() const; ///< \c true, if T bit is \e not set
                                        /**< Any multicast address with a cleared T bit must be
                                             globally assigned. See 
                                             <a href="http://tools.ietf.org/html/rfc4291">RFC 4291</a>. */
        bool prefixMulticastAddr() const; ///< \c true, if P bit is set
                                        /**< In <a href="http://tools.ietf.org/html/rfc4291">RFC 4291</a>,
                                             the P bit is specified as defining a
                                             unicast prefix based multicast address. See 
                                             <a href="http://tools.ietf.org/html/rfc3306">RFC 3306</a>. */ 
        bool embeddedRpAddr() const;    ///< \c true, if R bit is set
                                        /**< In <a href="http://tools.ietf.org/html/rfc4291">RFC 4291</a>,
                                             the R bit is specified as defining a multicast address 
                                             with embedded rendezvous point. See
                                             <a href="http://tools.ietf.org/html/rfc3956">RFC 3956</a>. */

        bool boolean_test() const;      ///< \c true, if address != '::' (None)

        ///@}
        ///\name Mutators
        ///@{
        
        void network(boost::uint64_t net); ///< Set network part of address
        void id(boost::uint64_t id);    ///< Set interface id part of address

        ///@}

        /** \brief Base-class for INet6Address exceptions */
        struct AddressException : public std::exception {};

        /** \brief Invalid INet4 address syntax */
        struct SyntaxException : public AddressException
        { virtual char const * what() const throw() 
                { return "invalid INet6 address syntax"; } };

        /** \brief Resolver failure */
        struct UnknownHostnameException : public AddressException
        { virtual char const * what() const throw() 
                { return "failed to resolve INet6 hostname"; } };
    };

    /** \brief Output INet6Address instance as it's string representation
        \related INet6Address
     */
    std::ostream & operator<<(std::ostream & os, INet6Address const & addr);

    /** \brief Check INet6Address against a fixed network prefix
        
        This helper allows to easily and efficiently check an INet6Address against an arbitrary but
        constant network prefix. It takes from 1 to 8 arguments for the network address and an
        additional last argument providing the prefix length. So

        \par ""
            <tt>senf::CheckINet6Network<</tt> <i>addr_1</i> <tt>,</tt> <i>addr_2</i> <tt>,</tt>
            ... <tt>,</tt> <i>prefix_len</i> <tt>></tt>

        represents the network
        
        \par ""
            <i>addr_1</i> <tt>:</tt> <i>addr_2</i> <tt>:</tt> ... <tt>::/</tt> <i>prefix_len</i> .

        The class exposes a single static member <tt>match(</tt> <i>addr</i> <tt>)</tt> which
        matches the INet6Address \a addr against the prefix:
        
        \code
        if (senf::CheckINet6Network<0x2000u,0xDB8u,32u>::match(addr)) {
            // 'addr' is within in the 2001:db8::/32 documentation-only network
            ...
        }
        \endcode

        The code generated by this call is highly optimized and probably as efficient as it can get.
     */
    template <unsigned a0, unsigned a1, unsigned a2=0u, unsigned a3=0u, unsigned a4=0u,
              unsigned a5=0u, unsigned a6=0u, unsigned a7=0u, unsigned a8=0u>
    struct CheckINet6Network
        : public detail::CheckINet6Network_impl<a0,a1,a2,a3,a4,a5,a6,a7,a8>
    {};

    /** \brief IpV6 network prefix

        This class represents an IpV6 network prefix in CIDR notation. 
      */
    class INet6Network
        : public boost::equality_comparable<INet6Network>, 
          public comparable_safe_bool<INet6Network>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        INet6Network();                 ///< Construct empty (::/0) network
        INet6Network(INet6Address address, unsigned prefix_len);
                                        ///< Construct network from given address and prefix length
        explicit INet6Network(std::string s); ///< Construct network from CIDR notation

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        INet6Address const & address() const; ///< Get the network address
        unsigned prefix_len() const;    ///< Get the network prefix length

        bool boolean_test() const;      ///< \c true, if INet6Network is non-empty
        bool operator==(INet6Network const & other) const;
                                        ///< Compare two networks for equality
        
        bool match(INet6Address addr) const; ///< \c true, if the network includes \a addr
        bool match(INet6Network net) const; ///< \c true, if the network includes \a net
                                        /**< The is true, if \a net is sub-network (or the same as)
                                             \c this. */
        INet6Address host(boost::uint64_t id); ///< Return the host with the given id
                                        /**< Returns the host with the given number within the
                                             network. This call replaces the lower 64 bits of the
                                             network address with the given id. */

        INet6Network subnet(boost::uint64_t net, unsigned prefix_len);
                                        ///< Return the given subnet of \c this
                                        /**< The returned INet6Network will be a subnet of \c this
                                             with the given network number. The network number is
                                             comprised by the bits above \a prefix_len:
                                             \code
                                             INet6Network("2001:db8::/32").subnet(0x12u,40u) == INet6Network("2001:db8:1200::/40")
                                             INet6Network("2001:db8:1200::/40").subnet(0x2345,64u) == INet6Network("2001:db8:1200:2345::/64")
                                             \endcode 
                                             \param[in] net network number
                                             \param[in] prefix_len length of subnet prefix */

    protected:

    private:
        unsigned prefix_len_;
        INet6Address address_;
    };

    /** \brief Output INet6Network instance as it's string representation
        \related INet6Network
     */
    std::ostream & operator<<(std::ostream & os, INet6Network const & addr);
}

///////////////////////////////hh.e////////////////////////////////////////
#include "INet6Address.cci"
#include "INet6Address.ct"
#include "INet6Address.cti"
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
