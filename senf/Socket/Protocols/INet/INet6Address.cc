// $Id$
//
// Copyright (C) 2007
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
    \brief INet6Address non-inline non-template implementation */

#include "INet6Address.hh"
#include "INet6Address.ih"

// Custom includes
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <boost/lexical_cast.hpp>
#include <senf/Socket/Protocols/AddressExceptions.hh>
#include <senf/Socket/Protocols/Raw/MACAddress.hh>
#include <senf/Socket/Protocols/Raw/EUI64.hh>

//#include "INet6Address.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::INet6Address

prefix_ senf::INet6Address senf::INet6Address::from_string(std::string const & s,
                                                           Resolve_t resolve)
{
    struct in6_addr ina;
    if (::inet_pton(AF_INET6,s.c_str(),&ina) > 0)
        return senf::INet6Address::from_data(&ina.s6_addr[0]);

    if (s.empty())
        throw AddressSyntaxException() << ": empty string";

    int herr (0);

    // If available, we use the reentrant GNU variant. This has the additional advantage, that we
    // can explicitly ask for IPv4 addresses

#   ifdef __GLIBC__

    struct hostent entbuf;
    char buffer[4096];
    struct hostent * ent (0);
    ::gethostbyname2_r(s.c_str(), AF_INET6, &entbuf, buffer, sizeof(buffer), &ent, &herr);

#   else // ! __GLIBC__

#   ifdef _REENTRANT
    static boost::mutex mutex;
    boost::mutex::scoped_lock lock(mutex);
#   endif
    struct hostent * ent (::gethostbyname(s.c_str()));
    herr = h_errno;

#   endif // __GLIBC__

    if (ent && ent->h_addrtype == AF_INET6)
        // We are only interested in the first address ...
        return senf::INet6Address::from_data(
            &reinterpret_cast<in6_addr*>(*(ent->h_addr_list))->s6_addr[0]);

    if (resolve == ResolveINet4)
        return from_inet4address(INet4Address::from_string(s));
    else
        throw UnknownHostnameException(s);
}

prefix_ in6_addr senf:: INet6Address::toin6_addr() const {
    ::in6_addr ina;
    std::copy((*this).begin(), (*this).end(), &ina.s6_addr[0]);
    return ina;
}

prefix_ senf::INet6Address senf::INet6Address::from_mac(MACAddress const & mac)
{
    INet6Address addr;
    addr[0] = 0xfe;
    addr[1] = 0x80;
    addr[8] = mac[0] ^ 0x2;  // invert the "u" (universal/local) bit; see RFC 4291 Appx. A
    addr[9] = mac[1];
    addr[10] = mac[2];
    addr[11] = 0xff;
    addr[12] = 0xfe;
    addr[13] = mac[3];
    addr[14] = mac[4];
    addr[15] = mac[5];
    return addr;
}

prefix_ senf::INet6Address senf::INet6Address::from_eui64(EUI64 const & eui)
{
    INet6Address addr;
    addr[0] = 0xfe;
    addr[1] = 0x80;
    addr[8] = eui[0] ^ 0x2;  // invert the "u" (universal/local) bit; see RFC 4291 Appx. A
    std::copy(eui.begin()+1, eui.end(), addr.begin()+9);
    return addr;
}

prefix_ senf::EUI64 senf::INet6Address::id()
    const
{
    return EUI64::from_data(begin()+8);
}

prefix_ std::ostream & senf::operator<<(std::ostream & os, INet6Address const & addr)
{
    ::in6_addr ina;
    char buffer[5*8];
    std::copy(addr.begin(),addr.end(),&ina.s6_addr[0]);
    ::inet_ntop(AF_INET6,&ina,buffer,sizeof(buffer));
    buffer[sizeof(buffer)-1] = 0;
    os << buffer;
    return os;
}

prefix_ std::istream & senf::operator>>(std::istream & is, INet6Address & addr)
{
    std::string s;
    if (!(is >> s))
        return is;
    try {
        addr = INet6Address::from_string(s);
    }
    catch (AddressException &) {
        is.setstate(std::ios::failbit);
    }
    return is;
}

senf::INet6Address const senf::INet6Address::None;
senf::INet6Address const senf::INet6Address::Loopback   (0u,0u,0u,0u,0u,0u,0u,1u);
senf::INet6Address const senf::INet6Address::AllNodes   (0xFF02u,0u,0u,0u,0u,0u,0u,1u);
senf::INet6Address const senf::INet6Address::AllRouters (0xFF02u,0u,0u,0u,0u,0u,0u,2u);

///////////////////////////////////////////////////////////////////////////
// senf::INet6Network

prefix_ senf::INet6Network::INet6Network(std::string const & s)
{
    using boost::lambda::_1;
    using boost::lambda::_2;
    std::string::size_type i (s.find('/'));
    if (i == std::string::npos)
        throw AddressSyntaxException(s);
    try {
        prefix_len_ = boost::lexical_cast<unsigned>(std::string(s,i+1));
    } catch (boost::bad_lexical_cast const &) {
        throw AddressSyntaxException(s);
    }
    address_ = INet6Address::from_string(std::string(s, 0, i));
    detail::apply_mask(prefix_len_, address_.begin(), address_.end(), _1 &= _2);
}

prefix_ std::istream & senf::operator>>(std::istream & is, INet6Network & addr)
{
    std::string s;
    if (!(is >> s))
        return is;
    try {
        addr = INet6Network(s);
    }
    catch (AddressException &) {
        is.setstate(std::ios::failbit);
    }
    return is;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "INet6Address.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
