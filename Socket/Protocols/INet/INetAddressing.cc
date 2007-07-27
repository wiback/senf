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
#include <net/if.h>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/range.hpp>

//#include "INetAddressing.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::INet4Address

prefix_ senf::INet4SocketAddress::INet4SocketAddress(std::string const & addr)
{
    clear();
    unsigned i = addr.find(':');
    if (i == std::string::npos)
        throw SyntaxException();
    address(INet4Address::from_string(std::string(addr,0,i)));
    try {
        port(boost::lexical_cast< ::u_int16_t >(std::string(addr,i+1)));
    }
    catch (boost::bad_lexical_cast const &) {
        throw SyntaxException();
    }
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
// senf::INet6Address

prefix_ senf::INet6Address::INet6Address(std::string const & addr)
{
    if (inet_pton(AF_INET6,addr.c_str(),&addr_) <= 0)
        throw SyntaxException();
}

prefix_ senf::INet6Address::INet6Address(char const * addr)
{
    if (inet_pton(AF_INET6,addr,&addr_) <= 0)
        throw SyntaxException();
}

prefix_ void senf::INet6Address::clear()
{
    ::memset(&addr_,0,sizeof(addr_));
}

prefix_ std::string senf::INet6Address::address()
    const
{
    char buffer[8*5];
    BOOST_ASSERT( inet_ntop(AF_INET6, &addr_, buffer, sizeof(buffer)) );
    return std::string(buffer);
}

prefix_ bool senf::INet6Address::operator==(INet6Address const & other)
    const
{
    return ::memcmp(&addr_,&other.addr_,sizeof(addr_))==0;
}

prefix_ bool senf::INet6Address::operator!=(INet6Address const & other)
    const
{
    return !operator==(other);
}

///////////////////////////////////////////////////////////////////////////
// senf::INet6SocketAddress

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

prefix_ std::string senf::INet6SocketAddress::address()
    const
{
    std::stringstream ss;
    ss << '[' << host();
    if (sockaddr_.sin6_scope_id != 0)
        ss << '@' << iface()
    << "]:" << port();
    return ss.str();
}

prefix_ std::string senf::INet6SocketAddress::iface()
    const
{
    if (sockaddr_.sin6_scope_id == 0)
        return "";
    char buffer[IFNAMSIZ];
    BOOST_ASSERT( if_indextoname(sockaddr_.sin6_scope_id,buffer) );
    return std::string(buffer);
}

prefix_ void senf::INet6SocketAddress::assignAddr(std::string const & addr)
{
    // Format of addr: "[" address [ "@" interface ] "]" ":" port
    typedef boost::char_separator<char> separator;
    typedef boost::tokenizer<separator> tokenizer;
    // we don't add ':' to the list of separators since that would give as the IPv6 address
    // as a list of tokens. We just strip the : from the port number manually
    separator sep ("", "@[]");
    tokenizer tokens (addr, sep);
    tokenizer::iterator token (tokens.begin());
    if (token == tokens.end()
        || *token != "["
        || ++token == tokens.end()
        || inet_pton(AF_INET6, std::string(boost::begin(*token),boost::end(*token)).c_str(),
                     &sockaddr_.sin6_addr) <= 0
        || ++token == tokens.end())
        throw SyntaxException();
    if (*token == "@") {
        if (++token == tokens.end())
            throw SyntaxException();
        assignIface(std::string(boost::begin(*token),boost::end(*token)));
        if (++token == tokens.end()
            || *token != "]")
            throw SyntaxException();
    } else if (*token != "]")
        throw SyntaxException();
    if (++token == tokens.end()
        || *boost::begin(*token) != ':')
        throw SyntaxException();
    try {
        sockaddr_.sin6_port = htons(
            boost::lexical_cast<unsigned>(std::string(boost::next(boost::begin(*token)),
                                                      boost::end(*token))));
    } catch(boost::bad_lexical_cast const &) {
        throw SyntaxException();
    }
    if (++token != tokens.end())
        throw SyntaxException();
}

prefix_ void senf::INet6SocketAddress::assignIface(std::string const & iface)
{
    if (iface.empty())
        sockaddr_.sin6_scope_id = 0;
    else {
        sockaddr_.sin6_scope_id = if_nametoindex(iface.c_str());
        if (sockaddr_.sin6_scope_id == 0)
            throw SyntaxException();
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
