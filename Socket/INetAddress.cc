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

// Definition of non-inline non-template functions

#include "INetAddress.hh"
//#include "INetAddress.ih"

// Custom includes
#include <strstream>
#include <string.h>
#include <sys/socket.h>
#include <boost/lexical_cast.hpp>

//#include "INetAddress.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ satcom::lib::INet4Address::INet4Address()
{
    ::memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
}

prefix_ satcom::lib::INet4Address const &
satcom::lib::INet4Address::operator=(std::string const & address)
{
    ::memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    // TODO: gethostbyname einbauen
    unsigned i = address.find(':');
    if (i == std::string::npos)
        throw InvalidINetAddressException();
    if (::inet_aton(std::string(address,0,i).c_str(), &addr.sin_addr) == 0)
        throw InvalidINetAddressException();
    try {
        // Replace lexical_cast with strtoul ?
        addr.sin_port = htons(boost::lexical_cast< ::u_int16_t >(std::string(address,i+1)));
    } 
    catch (boost::bad_lexical_cast const & ex) {
        throw InvalidINetAddressException();
    }
    return *this;
}

prefix_ satcom::lib::INet4Address const &
satcom::lib::INet4Address::operator=(std::pair<std::string, unsigned> const & address)
{
    ::memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    // TODO: gethostbyname einbauen
    if (::inet_aton(address.first.c_str(), &addr.sin_addr) == 0)
        throw InvalidINetAddressException();
    addr.sin_port = htons(address.second);
    return *this;
}

prefix_ std::string satcom::lib::INet4Address::str()
    const
{
    std::stringstream s;
    s << host() << ':' << port();
    return s.str();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "INetAddress.mpp"


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
