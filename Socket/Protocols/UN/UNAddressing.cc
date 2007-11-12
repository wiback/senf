// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum NETwork research (NET)
//     David Wagner <david.wagner@fokus.fraunhofer.de>
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
#include <boost/filesystem/path.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////
prefix_ senf::UNSocketAddress::UNSocketAddress(boost::filesystem::path p)
{
	sockAddr.sun_family = AF_UNIX;
	strcpy(sockAddr.sun_path, p.string().c_str());
}

prefix_ senf::UNSocketAddress fromString(std::string s) {
    return senf::UNSocketAddress::UNSocketAddress(boost::filesystem::path(s));
}

prefix_ senf::UNSocketAddress fromPath(boost::filesystem::path p)
{
    return senf::UNSocketAddress::UNSocketAddress(p);
}

prefix_ std::string senf::UNSocketAddress::path()
        const
{
    return std::string(sockAddr.sun_path);
}

prefix_ sockaddr_un senf::UNSocketAddress::sockaddr()
{
    struct sockaddr_un out; 
    out.sun_family = sockAddr.sun_family;
    strncpy(out.sun_path, sockAddr.sun_path, sizeof( out.sun_path));
    return out; 
}

prefix_ sockaddr * senf::UNSocketAddress::sockaddr_p()
{
    return reinterpret_cast <struct sockaddr  *> (&sockAddr); 
}


prefix_ sockaddr const  * senf::UNSocketAddress::sockaddr_p()
    const
{
    return reinterpret_cast <struct sockaddr const  *> (&sockAddr); 
}

prefix_ unsigned senf::UNSocketAddress::sockaddr_len()
	const
{
    return sizeof(sockAddr);
}

prefix_ std::ostream & operator<<(std::ostream & os, senf::UNSocketAddress::UNSocketAddress const & addr){
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
