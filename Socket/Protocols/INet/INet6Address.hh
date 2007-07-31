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
#include <boost/function.hpp>
#include <boost/array.hpp>
#include "Utils/SafeBool.hh"
#include "INet4Address.hh"

//#include "INet6Address.mpp"
#include "INet6Address.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief IpV6 network address

        This implementation of an IpV6 address is based strictly on RFC 4291: Internet Protocol
        Version 6 (IPv6) Addressing Architecture. This class provides accessors to all the
        information fields defined in this document.

        The IpV6 addressing architecture however has several other components defined in other
        RFC's. These RFC's should be implemented in additional modules:

        \li RFC4193: Unique Local Addresses (ULA). Defines the fc00::/7 prefix
        \li RFC3306: Unicast-Prefix-based IPv6 Multicast Addresses. Defines the ff30::/12 prefix
        \li RFC3956: Embedding the Rendezvous Point (RP) Address in an IPv6 Multicast
            Address. Defines the ff70::/12 prefix
        \li RFC3056: Connection of IPv6 Domains via IPv4 Clouds. Defines 6to4 tunneling and the
            2002::/16 prefix
        \li RFC3849: IPv6 Address Prefix Reserved for Documentation. Defines the 2001:db8::/32
            prefix
        
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

        \see CheckINet6Network Helper to check address against an arbitrary fixed network prefix
        \ingroup addr_group

        \implementation We awkwardly need to use static named constructors (<tt>from_</tt> members)
            instead of ordinarily overloaded constructors for one simple reason: <tt>char *</tt>
            doubles as string literal and as arbitrary data iterator. The iterator constructor can
            therefore not be distinguished from initialization with a string literal. Therefore we
            need to disambiguate using the named constructors.
     */
    class INet6Address
        : public boost::array<boost::uint8_t,16>,
          public ComparableSafeBool<INet6Address>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::function<void (INet6Address const &)> Callback;
                                        ///< Callback for asynchronous from_string call

        static INet6Address const None;        ///< The empty (::0) address
        static INet6Address const Loopback;    ///< The loopback (::1) address
        static INet6Address const AllNodes;    ///< The 'all nodes' link-local multicast address
        static INet6Address const AllRouters;  ///< The 'all routers' link-local multicast address

        enum NoInit_t { noinit };
        enum Resolve_t { ResolveINet6, ResolveINet4 };

        /** \brief Possible scope values

            List of all possible scope values. This list includes all scope values defined for
            multicast addresses in RFC4291. The values \ref LinkScope, \ref SiteScope and \ref
            GlobalScope are also used with unicast addresses.
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

        static void from_string(std::string const & s, Callback const & cb, 
                                Resolve_t resolve = ResolveINet6);
                                        ///< Convert string to address (async/non-blocking)
                                        /**< This member works like
                                             from_string(std::string const &). However unlike
                                             from_string(std::string const &), this call will not
                                             block. Instead it will call \a cb passing the
                                             INet6Address instance as soon as the address has been
                                             resolved (which may be immediate if the address
                                             represents an IP literal). \par
                                             On error, the address passed to \a cb will be empty.
                                             \param[in] s Address literal or hostname
                                             \param[in] cb Callback to pass the address to 
                                             \param[in] resolve If this is set to \c ResolveINet4,
                                                 the call will additionally try to interpret \a s as
                                                 an IpV4 address if no valid IpV6 address is
                                                 found. The address will be returned as mapped IpV6
                                                 address.
                                             \fixme Implement */

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
                                             an IpV6-mapped IpV4 address (see RFC4291). \par
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
                                             globally assigned. See RFC4291. */
        bool prefixMulticastAddr() const; ///< \c true, if P bit is set
                                        /**< In RFC4291, the P bit is specified as defining a
                                             unicast prefix based multicast address. See RFC3306. */ 
        bool embeddedRpAddr() const;    ///< \c true, if R bit is set
                                        /** In RFC4291, the R bit is specified as defining a
                                            multicast address with embedded rendezvous point. See
                                            RFC3956. */

        bool boolean_test() const;      ///< \c true, if address != '::' (None)

        ///@}
        ///\name Mutators
        ///@{
        
        void network(boost::uint64_t net); ///< Set network part of address
        void id(boost::uint64_t id);    ///< Set interface id part of address

        ///@}

        /** \brief Invalid IpV6 address syntax */
        struct SyntaxException : public std::exception
        { virtual char const * what() const throw() { return "Invalid IpV6 address syntax"; } };
    };

    /** \brief Output INet6Address instance as it's string representation
        \related INet6Address
     */
    std::ostream & operator<<(std::ostream & os, INet6Address const & addr);

    /** \brief Check address against a fixed network prefix
        
        This helper allows to easily and efficiently check an address against an arbitrary network
        prefix:
        \code
        if (senf::CheckINet6Network<0x2000u,0xDB8u,32u>::match(addr)) {
            // 'addr' is within in the 2001:db8::/32 documentation-only network
            ...
        }
        \endcode

        The code generated by this call is highly optimized and as probably as efficient as it can
        get.

        \related INet6Address
     */
    template <unsigned a0, unsigned a1, unsigned a2=0u, unsigned a3=0u, unsigned a4=0u,
              unsigned a5=0u, unsigned a6=0u, unsigned a7=0u, unsigned a8=0u>
    struct CheckINet6Network
        : public detail::CheckINet6Network_impl<a0,a1,a2,a3,a4,a5,a6,a7,a8>
    {};

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
