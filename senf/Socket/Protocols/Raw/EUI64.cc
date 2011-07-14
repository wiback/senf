// $Id$
//
// Copyright (C) 2009
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
