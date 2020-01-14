//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
