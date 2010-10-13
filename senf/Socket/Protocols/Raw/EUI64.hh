// $Id$
//
// Copyright (C) 2009
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
    \brief EUI64 public header */

#ifndef HH_SENF_Socket_Protocols_Raw_EUI64_
#define HH_SENF_Socket_Protocols_Raw_EUI64_ 1

// Custom includes
#include <iostream>
#include <boost/cstdint.hpp>
#include <boost/array.hpp>
#include <senf/Utils/Tags.hh>
#include <senf/Utils/safe_bool.hh>

//#include "EUI64.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    class MACAddress;

    /** \brief EUI-64 data type

        An EUI-64 is a 64 bit (8 octet) id. The id is represented as an 8 byte sequence in network
        byte order. An EUI64 can be converted from/to several other representations

        <table class="senf">
        <tr><td><tt>boost::uint64_t</tt></td>
                <td><tt>senf::EUI64(0x1a2b3c4d5f607182ull)</tt><br/>
                    <i>eui64</i><tt>.uint64()</tt></td></tr>
        <tr><td><tt>std::string</tt></td>
                <td><tt>senf::EUI64::from_string("1a:2b:3c:4d-5f:60:71:82")</tt><br/>
                    <tt>senf::str(</tt><i>eui64</i><tt>)</tt></td></tr>
        <tr><td><i>raw data</i><br/>&nbsp;&nbsp;&nbsp;&nbsp;(8 bytes)</td>
                <td><tt>senf::EUI64::from_data(</tt><i>iterator</i><tt>)</tt><br/>
                    <i>eui64</i><tt>.begin()</tt></td></tr>
        <tr><td>senf::MACAddress<br/>&nbsp;&nbsp;&nbsp;&nbsp;(aka EUI-48)</td>
                <td><tt>senf::EUI64::from_mac(</tt><i>mac-address</i><tt>)</tt><br/>
                    <tt>senf::MACAddress::from_eui64(</tt><i>eui64</i><tt>)</tt></td></tr>
        </table>

        Since senf::EUI64 is based on \c boost::array, you can access the raw data bytes of the
        address using \c begin(), \c end() or \c operator[]:
        \code
        senf::EUI64 eui64 (...);
        std::vector<char> data;
        data.resize(8);
        std::copy(eui64.begin(), eui64.end(), data.begin()); // Copy 8 bytes
        \endcode

        \see <a href="http://tools.ietf.org/html/rfc4291">RFC 4291</a>

        \ingroup addr_group
     */
    class EUI64
        : public boost::array<boost::uint8_t,8>,
          public senf::comparable_safe_bool<EUI64>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        static EUI64 const None;        ///< The empty (0) address

        // default copy constructor
        // default copy assignment
        // default destructor
        // no conversion constructors

        explicit EUI64(boost::uint64_t v=0u); ///< Construct EUI-64
        explicit EUI64(senf::NoInit_t);       ///< Construct <em>uninitialized</em> EUI-64

        static EUI64 from_mac(MACAddress const & mac);
                                        ///< Construct EUI-64 from senf::MACAddress
        static EUI64 from_string(std::string const & s);
                                        ///< Construct EUI-64 from string representation
                                        /**< The string representation consists of 8 octets in
                                             hexadecimal notation separated by ':' or '-'
                                             \throws senf::AddressSyntaxException */
        template <class InputIterator>
        static EUI64 from_data(InputIterator i);
                                        ///< Construct EUI-64 from 8 data octets
                                        /**< The iterator \a i must point to a sequence of 8
                                             octets in network byte order. */

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        bool isMACCompatible() const;   ///< \c true, if EUI64 is MAC compatible, \c false otherwise
                                        /**< An EUI64 is MAC compatible if bytes 4th and 5th byte
                                             (in network byte order) are 0xfffe. */
        bool local() const;             ///< \c true if the \e local bit is set, \c false otherwise
                                        /**< The \e local bit is the second least significant bit of
                                             the first octet (bit 6 in standard RFC bit numbering).
                                          */
        bool group() const;             ///< \c true if the \e group bit is set, \c false otherwise
                                        /**< The \e group bit is the least significant bit of the
                                             first octet (bit 7 in standard RFC bit numbering). */
        bool boolean_test() const;      ///< \c true, if EUI64 is != 0, \c false otherwise
        boost::uint64_t uint64() const; ///< Return EUI64 as integer number
    };

    /** \brief Write out EUI64 in it's string representation to stream
        \related senf::EUI64
     */
    std::ostream & operator<<(std::ostream & os, EUI64 const & v);

    /** \brief Read EUI64 string representation from stream
        \related senf::EUI64
     */
    std::istream & operator>>(std::istream & is, EUI64 & v);

}

///////////////////////////////hh.e////////////////////////////////////////
#include "EUI64.cci"
//#include "EUI64.ct"
#include "EUI64.cti"
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
