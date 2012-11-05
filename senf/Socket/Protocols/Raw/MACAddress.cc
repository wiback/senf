// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief MACAddress non-inline non-template implementation */

#include "MACAddress.hh"
//#include "MACAddress.ih"

// Custom includes
#include <iomanip>
#include <boost/io/ios_state.hpp>
#include <senf/Socket/Protocols/AddressExceptions.hh>
#include "ParseString.hh"
#include "EUI64.hh"

//#include "MACAddress.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::MACAddress

prefix_ senf::MACAddress senf::MACAddress::from_string(std::string const & s)
{
    MACAddress mac (senf::noinit);
    detail::parseHexString(s, ":-", mac.begin(), mac.end());
    return mac;
}

prefix_ senf::MACAddress senf::MACAddress::from_eui64(senf::EUI64 const & eui)
{
    if (eui[3] != 0xffu || eui[4] != 0xfeu)
        throw AddressSyntaxException() << ": EUI64 is not MAC compatible: " << eui;
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


//-/////////////////////////////////////////////////////////////////////////////////////////////////
// namespace members

prefix_ std::ostream & senf::operator<<(std::ostream & os, MACAddress const & mac)
{
    boost::io::ios_all_saver ias (os);
    os << std::hex << std::setfill('0') << std::right;
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
