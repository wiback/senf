// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fokus.fraunhofer.de.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Stefan Bund <senf@g0dil.de>

/** \file
    \brief MACAddress public header */

#ifndef HH_SENF_Socket_Protocols_Raw_MACAddress_
#define HH_SENF_Socket_Protocols_Raw_MACAddress_ 1

// Custom includes
#include <iosfwd>
#include <cstdint>
#include <boost/array.hpp>
#include <functional>
#include <senf/Utils/Tags.hh>
#include <senf/autoconf.hh>

//#include "MACAddress.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    struct EUI64;

    /** \brief Ethernet MAC address

        The Ethernet MAC is modeled as a fixed-size container/sequence of 6 bytes. A MACAddress can
        be converted from/to the following representations

        <table class="senf">
        <tr><td><tt>std::uint64_t</tt></td>
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
        : public boost::array<std::uint8_t,6>
    {
        static MACAddress const Broadcast; ///< The broadcast address
        static MACAddress const None;   ///< The empty (0) address

        // make sure we are compatible with older code expecting boost::array 
        // static constexpr unsigned static_size = 6;
        
        MACAddress();                   ///< Construct zero-initialized address
        MACAddress(senf::NoInit_t);     ///< Construct uninitialized (!) address
        explicit MACAddress(std::uint64_t const & v); ///< Construct MACAddress constants

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
                                             \throws AddressSyntaxException if \a eui is not a MAC
                                                 compatible EUI-64. */


        explicit operator bool() const;  ///< \c true, if address is not the zero address
        bool operator<(MACAddress const & other) const;
        bool operator==(EUI64 const & eui64) const;

        bool local() const;              ///< \c true, if address is locally administered
        bool multicast() const;          ///< \c true, if address is a group/multicast address
        bool broadcast() const;          ///< \c true, if address is the broadcast address

        void local(bool flag);
        void multicast(bool flag);

        std::uint32_t oui() const;    ///< Return first 3 bytes of the address
        std::uint32_t nic() const;    ///< Return last 3 bytes of the address

        void oui(std::uint32_t oui);  ///< Sets first 3 bytes of the address
        void nic(std::uint32_t oui);  ///< Sets last 3 bytes of the address
        
        std::uint64_t eui64() const;  ///< Build EUI-64 from the MAC address
        std::uint64_t uint64() const; ///< Return MAC address as uint64 value

        std::uint32_t const & hash32() const;
        void hash(std::uint64_t * hash, std::uint16_t const & otherKey = 0) const; ///< computes a fast uint64 hash mixing in another 16bit value
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
    std::istream & operator>>(std::istream & is, MACAddress & mac);

    bool operator==(EUI64 const & eui64, MACAddress const & mac);

# if __SIZEOF_SIZE_T__ == 8
    std::size_t hash_value(MACAddress const & mac) noexcept;
#elif __SIZEOF_SIZE_T__ == 4
    std::size_t const & hash_value(MACAddress const & mac) noexcept;
#else
    std::size_t hash_value(MACAddress const & mac) noexcept;    
#endif
}

#ifdef SENF_CXX11_ENABLED
namespace std {
    template <> struct hash<senf::MACAddress>
    {
# if __SIZEOF_SIZE_T__ == 8
        std::size_t operator()(senf::MACAddress const & mac) const noexcept {
            return senf::hash_value(mac);
        }
#elif  __SIZEOF_SIZE_T__ == 4
        std::size_t const & operator()(senf::MACAddress const & mac) const noexcept {
            return mac.hash32();
        }
#endif
    };
}
#endif

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
