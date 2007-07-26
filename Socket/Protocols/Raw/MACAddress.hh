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
    \brief MACAddress public header */

#ifndef HH_MACAddress_
#define HH_MACAddress_ 1

// Custom includes
#include <iostream>
#include <boost/cstdint.hpp>
#include <boost/array.hpp>
#include <boost/utility.hpp>
#include <boost/type_traits.hpp>
#include "Utils/SafeBool.hh"

//#include "MACAddress.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Ethernet MAC address
        
        The Ethernet MAC is modelled as a fixed-size container/sequence of 6 bytes.

        \implementation We awkwardly need to use static named constructors (<tt>from_</tt> members)
            instead of ordinarily overloaded constructors for one simple reason: <tt>char *</tt>
            doubles as string literal and as arbitrary data iterator. The iterator constructor can
            therefore not be distinguished from initialization with a string literal. Therefore we
            need to disambiguate using the named constructors.
     */
    struct MACAddress
        : public boost::array<boost::uint8_t,6>, 
          public ComparableSafeBool<MACAddress>
    {
        static MACAddress const Broadcast; ///< The broadcast address
        static MACAddress const None;   ///< The empty (0) address

        enum NoInit_t { noinit };
        
        MACAddress();                   ///< Construct zero-initialized address
        MACAddress(NoInit_t);           ///< Construct uninitialized (!) address
        explicit MACAddress(boost::uint64_t v); ///< Construct MACAddress constants

        static MACAddress from_string(std::string const & s);
                                        ///< Construct address from string representation
                                        /**< The string representation must exactly match the form
                                             <tt>dd:dd:dd:dd:dd:dd</tt> where <tt>d</tt> is any
                                             hexadecimal digit. In place of ':', '-' is also
                                             accepted as a delimiter. */

        template <class InputIterator> 
        static MACAddress from_data(InputIterator i);
                                        ///< Construct address from raw data
                                        /**< Copies the data from \a i into the MAC address.
                                             \pre The input range at \a i must have a size of at
                                                 least 6 elements. */

        static MACAddress from_eui64(boost::uint64_t v);
                                        ///< Construct address from EUI-64
                                        /**< This constructor takes an EUI-64 value and converts it
                                             to a MAC address. This conversion is only possible, if
                                             the EUI-64 is MAC compatible: the 4th/5th byte (in
                                             transmission order) must be 0xFFFE.
                                             \throws SyntaxException if \a v is not a MAC compatible
                                                 EUI-64. */

        bool local() const;             ///< \c true, if address is locally administered
        bool multicast() const;             ///< \c true, if address is a group/multicast address
        bool broadcast() const;         ///< \c true, if address is the broadcast address
        bool boolean_test() const;      ///< \c true, if address is the zero address

        boost::uint32_t oui() const;    ///< Return first 3 bytes of the address
        boost::uint32_t nic() const;    ///< Return last 3 bytes of the address
        
        boost::uint64_t eui64() const;  ///< Build EUI-64 from the MAC address

        /** \brief Bad MAC address syntax or conversion */
        struct SyntaxException : public std::exception
        { virtual char const * what() const throw() { return "invalid MAC address syntax"; } };
    };

    /** \brief Write MAC address
        \related MACAddress
     */
    std::ostream & operator<<(std::ostream & os, MACAddress const & mac);

}

///////////////////////////////hh.e////////////////////////////////////////
#include "MACAddress.cci"
#include "MACAddress.ct"
//#include "MACAddress.cti"
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
