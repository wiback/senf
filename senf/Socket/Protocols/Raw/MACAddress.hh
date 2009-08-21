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
    \brief MACAddress public header */

#ifndef HH_SENF_Socket_Protocols_Raw_MACAddress_
#define HH_SENF_Socket_Protocols_Raw_MACAddress_ 1

// Custom includes
#include <iostream>
#include <boost/cstdint.hpp>
#include <boost/array.hpp>
#include <boost/utility.hpp>
#include <boost/type_traits.hpp>
#include <senf/Utils/safe_bool.hh>
#include <senf/Utils/Tags.hh>
#include <senf/Socket/Protocols/AddressExceptions.hh>

//#include "MACAddress.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    class EUI64;

    /** \brief Ethernet MAC address

        The Ethernet MAC is modelled as a fixed-size container/sequence of 6 bytes. A MACAddress can
        be converted from/to the following representations

        <table class="senf">
        <tr><td><tt>boost::uint64_t</tt></td>
                <td><tt>senf::MACAddress(0x112233445566ull)</tt><br/>
                    <i>mac</i><tt>.uint64()</tt></td></tr>
        <tr><td><tt>std::string</tt></td>
                <td><tt>senf::MACAddress::from_string("11:22:33:44:55:66")</tt><br/>
                    <tt>senf::str(</tt><i>mac</i><tt>)</tt></td></tr>
        <tr><td><i>raw data</i><br/>&nbsp;&nbsp;&nbsp;&nbsp;(6 bytes)</td>
                <td><tt>senf::MACAddress::from_data(</tt><i>iterator</i><tt>)</tt><br/>
                    <i>mac</i><tt>.begin()</tt></td></tr>
        <tr><td>senf::EUI64</td>
                <td><tt>senf::MACAddress::from_eui64(</tt><i>eui64</i><tt>)</tt><br/>
                    <tt>senf::EUI64::from_mac(</tt><i>mac</i><tt>)</tt></td></tr>
        </table>

        Since MACAddress is based on \c boost::array, you can access the raw data bytes of the
        address using \c begin(), \c end() or \c operator[]:
        \code
        senf::MACAddress mac (...);
        std::vector<char> data;
        data.resize(6);
        std::copy(mac.begin(), mac.end(), data.begin()); // Copy 6 bytes
        \endcode

        \implementation We awkwardly need to use static named constructors (<tt>from_</tt> members)
            instead of ordinarily overloaded constructors for one simple reason: <tt>char *</tt>
            doubles as string literal and as arbitrary data iterator. The iterator constructor can
            therefore not be distinguished from initialization with a string literal. Therefore we
            need to disambiguate using the named constructors.

        \ingroup addr_group
     */
    struct MACAddress
        : public boost::array<boost::uint8_t,6>,
          public comparable_safe_bool<MACAddress>
    {
        static MACAddress const Broadcast; ///< The broadcast address
        static MACAddress const None;   ///< The empty (0) address

        MACAddress();                   ///< Construct zero-initialized address
        MACAddress(senf::NoInit_t);     ///< Construct uninitialized (!) address
        explicit MACAddress(boost::uint64_t v); ///< Construct MACAddress constants

        static MACAddress from_string(std::string const & s);
                                        ///< Construct address from string representation
                                        /**< The string representation must exactly match the form
                                             <tt>dd:dd:dd:dd:dd:dd</tt> where <tt>d</tt> is any
                                             hexadecimal digit. In place of ':', '-' is also
                                             accepted as a delimiter.
                                             \throws AddressSyntaxException */

        template <class InputIterator>
        static MACAddress from_data(InputIterator i);
                                        ///< Construct address from raw data
                                        /**< Copies the data from \a i into the MAC address.
                                             \pre The input range at \a i must have a size of at
                                                 least 6 elements. */

        static MACAddress from_eui64(senf::EUI64 const & eui);
                                        ///< Construct address from EUI-64
                                        /**< This constructor takes an EUI-64 value and converts it
                                             to a MAC address. This conversion is only possible, if
                                             the EUI-64 is MAC compatible: the 4th/5th byte (in
                                             transmission order) must be 0xFFFE.
                                             \throws AddressSyntaxException if \a v is not a MAC
                                                 compatible EUI-64. */

        bool local() const;             ///< \c true, if address is locally administered
        bool multicast() const;         ///< \c true, if address is a group/multicast address
        bool broadcast() const;         ///< \c true, if address is the broadcast address
        bool boolean_test() const;      ///< \c true, if address is not the zero address

        boost::uint32_t oui() const;    ///< Return first 3 bytes of the address
        boost::uint32_t nic() const;    ///< Return last 3 bytes of the address

        boost::uint64_t eui64() const;  ///< Build EUI-64 from the MAC address
        boost::uint64_t uint64() const; ///< Return MAC address as uint64 value
    };

    /** \brief Output MAC instance as it's string representation
        \related MACAddress
     */
    std::ostream & operator<<(std::ostream & os, MACAddress const & mac);
    /** \brief Try to initialize MACAddress instance from a string representation
        sets std::ios::failbit on the stream if an error occurred
        \see MACAddress from_string()
        \related MACAddress
     */
    std::istream & operator>>(std::istream & os, MACAddress & mac);

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
