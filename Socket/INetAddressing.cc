// $Id$
//
// Copyright (C) 2006 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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
    \brief INet[46]Address and INet[46]AddressingPolicy non-inline non-template implementation
 */

#include "INetAddressing.hh"
//#include "INetAddressing.ih"

// Custom includes
#include <sstream>
#include <string.h>
#include <sys/socket.h>
#include <boost/lexical_cast.hpp>

//#include "INetAddressing.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::INet4Address::INet4Address(std::string host, unsigned port)
{
    clear();
    /** \todo  gethostbyname support */
    if (::inet_aton(host.c_str(), &addr_.sin_addr) == 0)
        throw InvalidINetAddressException();
    addr_.sin_port = htons(port);
}

prefix_ std::string senf::INet4Address::str()
    const
{
    std::stringstream s;
    s << host() << ':' << port();
    return s.str();
}

prefix_ void senf::INet4Address::clear()
{
    ::memset(&addr_,0,sizeof(addr_));
    addr_.sin_family = AF_INET;
}

prefix_ void senf::INet4Address::assignString(std::string address)
{
    clear();
    unsigned i = address.find(':');
    if (i == std::string::npos)
        throw InvalidINetAddressException();
    // The temporary string in the next expr is guaranteed to live
    // until end-of-statement
    if (::inet_aton(std::string(address,0,i).c_str(), &addr_.sin_addr) == 0)
        throw InvalidINetAddressException();
    try {
        // Replace lexical_cast with strtoul ?
        addr_.sin_port = htons(boost::lexical_cast< ::u_int16_t >(std::string(address,i+1)));
    } 
    catch (boost::bad_lexical_cast const &) {
        throw InvalidINetAddressException();
    }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "INetAddressing.mpp"


// Local Variables:
// mode: c++
// c-file-style: "senf"
// End:
