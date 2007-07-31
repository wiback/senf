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
    \brief INet4Address public header */

#ifndef HH_INet4Address_
#define HH_INet4Address_ 1

// Custom includes
#include <iostream>
#include <string>
#include <boost/cstdint.hpp>
#include <boost/function.hpp>
#include <boost/array.hpp>
#include "Utils/SafeBool.hh"

//#include "INet4Address.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
    
    /** \brief IpV4 Internet address
        
        INet4Address represents a simple IP address. It is modelled as a fixed-size
        container/sequence of 4 bytes.

        \todo Add additional classes for CIDR addresses and networks and network math.
        \ingroup addr_group
      */
    class INet4Address
        : public boost::array<boost::uint8_t,4>, 
          public ComparableSafeBool<INet4Address>

    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types
        
        typedef uint32_t address_type;  ///< Address representation as number in host byte order
        typedef uint32_t inaddr_type;   ///< Legacy address representation in network byte order
        typedef boost::function<void (INet4Address const &)> Callback;
                                        ///< Callback for asynchronous from_string call

        static INet4Address const None; ///< The empty (0) address
        static INet4Address const Loopback; ///< The loopback (127.0.0.1) address
        static INet4Address const Broadcast; ////< The global broadcast (255.255.255.255) address

        enum NoInit_t { noinit };

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        INet4Address();                 ///< Construct an empty address
        explicit INet4Address(NoInit_t); ///< Construct uninitialized (!) address
        explicit INet4Address(address_type value);
                                        ///< Construct an address constant

        static INet4Address from_string(std::string const & s);
                                        ///< Convert string to address
                                        /**< This member will try to convert the given string into
                                             an IP address. from_string() supports all standard IP
                                             literal representations as well es hostnames.
                                             \attention This call may block if \a s represents a
                                                 hostname which must be looked up via some network
                                                 protocol like DNS or NIS
                                             \throws SyntaxException if the address cannot be
                                                 converted for some reason
                                             \param[in] s Address literal or hostname */
        
        static void from_string(std::string const & s, Callback const & cb);
                                        ///< Convert string to address (async/non-blocking)
                                        /**< This member works like
                                             from_string(std::string const &). However unlike
                                             from_string(std::string const &), this call will not
                                             block. Instead it will call \a cb passing the
                                             INet4Address instance as soon as the address has been
                                             resolved (which may be immediate if the address
                                             represents an IP literal). \par
                                             On error, the address passed to \a cb will be empty.
                                             \param[in] s Address literal or hostname
                                             \param[in] cb Callback to pass the address to 
                                             \fixme Implement */

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

        struct SyntaxException : public std::exception
        { virtual char const * what() const throw() { return "invalid INet4 address syntax"; } };

    private:
        enum InAddr_t { IsInAddr };
        INet4Address(inaddr_type addr, InAddr_t);
        inaddr_type & iref();
        inaddr_type iref() const;
    };

    std::ostream & operator<<(std::ostream & os, INet4Address const & addr);

}

///////////////////////////////hh.e////////////////////////////////////////
#include "INet4Address.cci"
#include "INet4Address.ct"
//#include "INet4Address.cti"
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
