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
    \brief EUI64 non-inline non-template implementation */

#include "EUI64.hh"
//#include "EUI64.ih"

// Custom includes
#include <boost/io/ios_state.hpp>
#include <iomanip>
#include "ParseString.hh"

//#include "EUI64.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

senf::EUI64 const senf::EUI64::None;

prefix_ senf::EUI64 senf::EUI64::from_string(std::string const & s)
{
    EUI64 eui (senf::noinit);
    detail::parseHexString(s, ":-", eui.begin(), eui.end());
    return eui;
}

prefix_ std::ostream & senf::operator<<(std::ostream & os, EUI64 const & eui)
{
    boost::io::ios_all_saver ias(os);
    os << std::hex << std::setfill('0') << std::right
       << std::setw(2) << unsigned(eui[0]) << ":"
       << std::setw(2) << unsigned(eui[1]) << ":"
       << std::setw(2) << unsigned(eui[2]) << ":"
       << std::setw(2) << unsigned(eui[3]) << "-"
       << std::setw(2) << unsigned(eui[4]) << ":"
       << std::setw(2) << unsigned(eui[5]) << ":"
       << std::setw(2) << unsigned(eui[6]) << ":"
       << std::setw(2) << unsigned(eui[7]);
    return os;
}

prefix_ std::istream & senf::operator>>(std::istream & is, EUI64 & eui)
{
    std::string s;
    if (!(is >> s))
        return is;
    try {
        eui = EUI64::from_string(s);
    }
    catch (AddressException &) {
        is.setstate(std::ios::failbit);
    }
    return is;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "EUI64.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
