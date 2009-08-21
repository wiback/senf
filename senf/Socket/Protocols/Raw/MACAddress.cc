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
    \brief MACAddress non-inline non-template implementation */

#include "MACAddress.hh"
//#include "MACAddress.ih"

// Custom includes
#include <iomanip>
#include <string>
#include <sstream>
#include <boost/io/ios_state.hpp>
#include "ParseString.hh"
#include "EUI64.hh"

//#include "MACAddress.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::MACAddress

prefix_ senf::MACAddress::MACAddress senf::MACAddress::from_string(std::string const & s)
{
    MACAddress mac (senf::noinit);
    detail::parseHexString(s, ":-", mac.begin(), mac.end());
    return mac;
}

prefix_ senf::MACAddress senf::MACAddress::from_eui64(senf::EUI64 const & eui)
{
    if (eui[3] != 0xffu || eui[4] != 0xfeu)
        throw AddressSyntaxException();
    MACAddress mac (senf::noinit);
    mac[0] = eui[0];
    mac[1] = eui[1];
    mac[2] = eui[2];
    mac[3] = eui[5];
    mac[4] = eui[6];
    mac[5] = eui[7];
    return mac;
}

senf::MACAddress const senf::MACAddress::Broadcast = senf::MACAddress(0xFFFFFFFFFFFFull);
senf::MACAddress const senf::MACAddress::None;


///////////////////////////////////////////////////////////////////////////
// namespace members

prefix_ std::ostream & senf::operator<<(std::ostream & os, MACAddress const & mac)
{
    boost::io::ios_all_saver ias (os);
    os << std::hex << std::setfill('0');
    for (MACAddress::const_iterator i (mac.begin()); i != mac.end(); ++i) {
        if (i != mac.begin())
            os << ':';
        os << std::setw(2) << unsigned(*i);
    }
    return os;
}

prefix_ std::istream & senf::operator>>(std::istream & is, MACAddress & mac)
{
    std::string s;
    if (!(is >> s))
        return is;
    try {
        mac = MACAddress::from_string(s);
    }
    catch (AddressException &) {
        is.setstate(std::ios::failbit);
    }
    return is;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "MACAddress.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
