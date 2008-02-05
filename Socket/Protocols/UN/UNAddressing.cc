// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     David Wagner <dw6@berlios.de>
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
    \brief UNAddressing non-inline non-template implementation */

#include "UNAddressing.hh"
//#include "UNAddressing.ih"

// Custom includes
#include <stdio.h>
#include <boost/operators.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::UNSocketAddress::UNSocketAddress()
{}

prefix_ senf::UNSocketAddress::UNSocketAddress(std::string p)
{
    clear();
    ::strncpy(addr_.sun_path, p.c_str(), sizeof(addr_.sun_path));
    addr_.sun_path[sizeof(addr_.sun_path)-1] = 0;
}

prefix_ senf::UNSocketAddress fromString(std::string s) 
{
    return senf::UNSocketAddress::UNSocketAddress(s);
}

prefix_ bool senf::UNSocketAddress::operator==(UNSocketAddress const & other)
    const
{
    return path() == other.path();
}

prefix_ std::string senf::UNSocketAddress::path()
        const
{
    return std::string(addr_.sun_path);
}

prefix_ bool senf::UNSocketAddress::boolean_test()
    const
{
    return addr_.sun_path[0] != 0;
}

prefix_ void senf::UNSocketAddress::clear()
{
    ::memset(&addr_, 0, sizeof(addr_));
    addr_.sun_family = AF_UNIX;
}

prefix_ sockaddr * senf::UNSocketAddress::sockaddr_p()
{
    return reinterpret_cast <struct sockaddr  *> (&addr_); 
}

prefix_ sockaddr const  * senf::UNSocketAddress::sockaddr_p()
    const
{
    return reinterpret_cast <struct sockaddr const  *> (&addr_); 
}

prefix_ unsigned senf::UNSocketAddress::sockaddr_len()
    const
{
    return sizeof(addr_);
}

prefix_ std::ostream & operator<<(std::ostream & os,
                                  senf::UNSocketAddress::UNSocketAddress const & addr)
{
    os << addr.path();
    return os;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "UNAddressing.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
