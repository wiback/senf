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
#include <senf/Utils/senfassert.hh>
#include <senf/Socket/Protocols/AddressExceptions.hh>

//#include "INetAddressing.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::INet4SocketAddress

prefix_ senf::INet4SocketAddress::INet4SocketAddress(std::string const & addr)
    : BSDSocketAddress (sizeof(sockaddr_in), AF_INET)
{
    std::string::size_type portIx = addr.find(':');
    try {
        port( boost::lexical_cast< ::uint16_t>(portIx == std::string::npos ? addr : std::string(addr,portIx+1)) );
    }
    catch (boost::bad_lexical_cast const &) {
        throw AddressSyntaxException(addr) << ": invalid port number";
    }
    if (portIx != std::string::npos)
        address( INet4Address::from_string(std::string(addr,0,portIx)) );
}

prefix_ senf::INet4SocketAddress::INet4SocketAddress(INet4Address const & addr, unsigned p)
    : BSDSocketAddress (sizeof(sockaddr_in), AF_INET)
{
    address(addr);
    port(p);
}

prefix_ senf::INet4SocketAddress::INet4SocketAddress(unsigned p)
    : BSDSocketAddress (sizeof(sockaddr_in), AF_INET)
{
    port(p);
}

prefix_ std::ostream & senf::operator<<(std::ostream & os, INet4SocketAddress const & addr)
{
    os << addr.address() << ":" << addr.port();
    return os;
}

prefix_ std::istream & senf::operator>>(std::istream & is, INet4SocketAddress & addr)
{
    std::string s;
    if (!(is >> s))
        return is;
    try {
        addr = INet4SocketAddress(s);
    }
    catch (AddressException &) {
        is.setstate(std::ios::failbit);
    }
    return is;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::INet6SocketAddress

prefix_ senf::INet6SocketAddress::INet6SocketAddress(std::string const & addr,
                                                     INet6Address::Resolve_t resolve)
    : BSDSocketAddress (sizeof(sockaddr_in6), AF_INET6)
{
    // Format of addr: "[" address [ "%" interface ] "]" ":" port
    //             or: host ":" port
    //             or: port

    static boost::regex const addressRx ("(?:(?:\\[([^%]+)(?:%(.+))?\\]|(.+)):)?([0-9]+)");
    // Subexpression numbers:
    enum { Address  = 1,
           ZoneId   = 2,
           Hostname = 3,
           Port     = 4 };

    boost::smatch match;
    if (! regex_match(addr, match, addressRx))
        throw AddressSyntaxException(addr);

    if (match[ZoneId].matched)
        assignIface(match[ZoneId]);

    sockaddr_.sin6_port = htons(boost::lexical_cast<boost::uint16_t>(match[Port]));

    if (match[Address].matched || match[Hostname].matched) {
        INet6Address a (INet6Address::from_string(
                            match[Address].matched ? match[Address] : match[Hostname],
                            resolve));
        std::copy(a.begin(), a.end(), &sockaddr_.sin6_addr.s6_addr[0]);
    }
}

prefix_ std::string senf::INet6SocketAddress::iface()
    const
{
    if (sockaddr_.sin6_scope_id == 0)
        return "";
    char buffer[IFNAMSIZ];
    SENF_ASSERT_EXPRESSION(if_indextoname(sockaddr_.sin6_scope_id,buffer),
                           "Internal failure: Invalid interface index (how does it get here?)");
    return std::string(buffer);
}

prefix_ void senf::INet6SocketAddress::assignIface(std::string const & iface)
{
    if (iface.empty())
        sockaddr_.sin6_scope_id = 0;
    else {
        sockaddr_.sin6_scope_id = if_nametoindex(iface.c_str());
        if (sockaddr_.sin6_scope_id == 0)
            throw AddressSyntaxException(iface);
    }
}

prefix_ std::ostream & senf::operator<<(std::ostream & os, INet6SocketAddress const & addr)
{
    os << '[' << addr.address();
    std::string iface (addr.iface());
    if (! iface.empty())
        os << '%' << iface;
    os << "]:" << addr.port();
    return os;
}

prefix_ std::istream & senf::operator>>(std::istream & is, INet6SocketAddress & addr)
{
    std::string s;
    if (!(is >> s))
        return is;
    try {
        addr = INet6SocketAddress(s);
    }
    catch (AddressException &) {
        is.setstate(std::ios::failbit);
    }
    return is;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
