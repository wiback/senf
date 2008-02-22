// $Id$
//
// Copyright (C) 2006
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
    \brief INet[46]Address and INet[46]AddressingPolicy non-inline non-template implementation
 */

#include "INetAddressing.hh"
//#include "INetAddressing.ih"

// Custom includes
#include <sstream>
#include <string.h>
#include <sys/socket.h>
#include <net/if.h>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include "../../../Utils/senfassert.hh"

//#include "INetAddressing.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::INet4SocketAddress

prefix_ senf::INet4SocketAddress::INet4SocketAddress(std::string const & addr)
{
    clear();
    unsigned i = addr.find(':');
    if (i == std::string::npos)
        throw AddressSyntaxException();
    try {
        port(boost::lexical_cast< ::u_int16_t >(std::string(addr,i+1)));
    }
    catch (boost::bad_lexical_cast const &) {
        throw AddressSyntaxException();
    }
    address(INet4Address::from_string(std::string(addr,0,i)));
}

prefix_ senf::INet4SocketAddress::INet4SocketAddress(INet4Address const & addr, unsigned p)
{
    clear();
    address(addr);
    port(p);
}

prefix_ void senf::INet4SocketAddress::clear()
{
    ::memset(&addr_,0,sizeof(addr_));
    addr_.sin_family = AF_INET;
}

///////////////////////////////////////////////////////////////////////////
// senf::INet6SocketAddress

prefix_ senf::INet6SocketAddress::INet6SocketAddress(std::string const & addr,
                                                     INet6Address::Resolve_t resolve)
{
    clear();

    // Format of addr: "[" address [ "%" interface ] "]" ":" port
    //             or: host ":" port

    static boost::regex const addressRx ("(?:\\[([a-f0-9A-F:]+)(?:%(.+))?\\]|(.+)):([0-9]+)");
    // Subexpression numbers:
    enum { NumericAddr = 1,
           ZoneId      = 2,
           Hostname    = 3,
           Port        = 4 };
    
    boost::smatch match;
    if (! regex_match(addr, match, addressRx))
        throw AddressSyntaxException();

    if (match[ZoneId].matched)
        assignIface(match[ZoneId]);

    sockaddr_.sin6_port = htons(boost::lexical_cast<boost::uint16_t>(match[Port]));

    INet6Address a (INet6Address::from_string(
                        match[NumericAddr].matched ? match[NumericAddr] : match[Hostname],
                        resolve));
    std::copy(a.begin(), a.end(), &sockaddr_.sin6_addr.s6_addr[0]);
}

prefix_ bool senf::INet6SocketAddress::operator==(INet6SocketAddress const & other)
    const
{
    return ::memcmp(&sockaddr_.sin6_addr, &other.sockaddr_.sin6_addr, sizeof(sockaddr_.sin6_addr))==0 &&
        sockaddr_.sin6_port == other.sockaddr_.sin6_port &&
        sockaddr_.sin6_scope_id == other.sockaddr_.sin6_scope_id;
}

prefix_ bool senf::INet6SocketAddress::operator!=(INet6SocketAddress const & other)
    const
{
    return ! operator==(other);
}

prefix_ void senf::INet6SocketAddress::clear()
{
    ::memset(&sockaddr_,0,sizeof(sockaddr_));
    sockaddr_.sin6_family = AF_INET6;
}

prefix_ std::string senf::INet6SocketAddress::iface()
    const
{
    if (sockaddr_.sin6_scope_id == 0)
        return "";
    char buffer[IFNAMSIZ];
#ifdef SENF_DEBUG
    SENF_ASSERT( if_indextoname(sockaddr_.sin6_scope_id,buffer) );
#else
    if_indextoname(sockaddr_.sin6_scope_id,buffer);
#endif
    return std::string(buffer);
}

prefix_ void senf::INet6SocketAddress::assignIface(std::string const & iface)
{
    if (iface.empty())
        sockaddr_.sin6_scope_id = 0;
    else {
        sockaddr_.sin6_scope_id = if_nametoindex(iface.c_str());
        if (sockaddr_.sin6_scope_id == 0)
            throw AddressSyntaxException();
    }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "INetAddressing.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
