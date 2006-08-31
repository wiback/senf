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

prefix_ satcom::lib::INet4Address::INet4Address(std::string host, unsigned port)
{
    clear();
    // TODO: gethostbyname einbauen
    if (::inet_aton(host.c_str(), &addr.sin_addr) == 0)
        throw InvalidINetAddressException();
    addr.sin_port = htons(port);
}

prefix_ std::string satcom::lib::INet4Address::str()
    const
{
    std::stringstream s;
    s << host() << ':' << port();
    return s.str();
}

prefix_ void satcom::lib::INet4Address::clear()
{
    ::memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
}

prefix_ void satcom::lib::INet4Address::assignString(std::string address)
{
    clear();
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
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "INetAddress.mpp"


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
