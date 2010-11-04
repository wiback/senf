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
    os << std::hex << std::setfill('0')
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
