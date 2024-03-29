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
    \brief INet4Address non-inline non-template implementation */

#include "INet4Address.hh"
//#include "INet4Address.ih"

// Custom includes
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <boost/lexical_cast.hpp>
#if defined(_REENTRANT) && !defined(__GLIBC__)
#include <boost/thread/mutex.hpp>
#endif

//#include "INet4Address.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::INet4Address::INet4Address

prefix_ senf::INet4Address::INet4Address(address_type value)
{
    iref() = htonl(value);
}

prefix_ senf::INet4Address senf::INet4Address::from_string(std::string const & s)
{
    struct in_addr ina;
    if (::inet_pton(AF_INET,s.c_str(),&ina) > 0)
        return INet4Address::from_inaddr(ina.s_addr);

    if  (s.empty())
        throw AddressSyntaxException() << ": empty string";

    // If available, we use the reentrant GNU variant. This has the additional advantage, that we
    // can explicitly ask for IPv4 addresses

#   ifdef __GLIBC__

    struct hostent entbuf;
    char buffer[4096];
    struct hostent * ent (0);
    int herr (0);
    ::gethostbyname2_r(s.c_str(), AF_INET, &entbuf, buffer, sizeof(buffer), &ent, &herr);

#   else // ! __GLIBC__

#   ifdef _REENTRANT
    static boost::mutex mutex;
    boost::mutex::scoped_lock lock(mutex);
#   endif
    struct hostent * ent (::gethostbyname(s.c_str()));
#   endif // __GLIBC__

    if (!ent)
        throw UnknownHostnameException(s);
    if (ent->h_addrtype != AF_INET)
        throw UnknownHostnameException(s);

    // We are only interested in the first address ...
    return INet4Address::from_inaddr(
        reinterpret_cast<in_addr*>(*(ent->h_addr_list))->s_addr);
}

prefix_ bool senf::INet4Address::local()
    const
{
    address_type l (address());
    return
        (l & 0xFF000000u) == 0x0A000000u ||
        (l & 0xFFF00000u) == 0xAC100000u ||
        (l & 0xFFFF0000u) == 0xA9FE0000u ||
        (l & 0xFFFF0000u) == 0xC0A80000u;
}

prefix_ bool senf::INet4Address::loopback()
    const
{
    return (address() & 0xFF000000u) == 0x7F000000u;
}

prefix_ bool senf::INet4Address::multicast()
    const
{
    return (address() & 0xF0000000u) == 0xE0000000u;
}

prefix_ senf::INet4Address::address_type senf::INet4Address::address()
    const
{
    return ntohl(iref());
}

senf::INet4Address const senf::INet4Address::None;
senf::INet4Address const senf::INet4Address::Loopback (0x7F000001u);
senf::INet4Address const senf::INet4Address::Broadcast (0xFFFFFFFFu);

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::INet4Network

prefix_ senf::INet4Network::INet4Network(std::string const & s)
{
    std::string::size_type i (s.find('/'));
    if (i == std::string::npos)
        throw AddressSyntaxException(s);
    try {
        prefix_len_ = prefix_len_checked(boost::lexical_cast<unsigned>(std::string(s,i+1)));
    } catch (boost::bad_lexical_cast const &) {
        throw AddressSyntaxException(s);
    }
    address_ = INet4Address(INet4Address::from_string(std::string(s, 0, i)).address() & mask());
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// namespace members

prefix_ std::ostream & senf::operator<<(std::ostream & os, INet4Address const & addr)
{
    ::in_addr ina;
    char buffer[16];
    ina.s_addr = addr.inaddr();
    ::inet_ntop(AF_INET,&ina,buffer,16);
    buffer[15] = 0;
    os << buffer;
    return os;
}

prefix_ std::istream & senf::operator>>(std::istream & is, INet4Address & addr)
{
    std::string s;
    if (!(is >> s))
        return is;
    try {
        addr = INet4Address::from_string(s);
    }
    catch (AddressException &) {
        is.setstate(std::ios::failbit);
    }
    return is;
}

prefix_ std::istream & senf::operator>>(std::istream & is, INet4Network & addr)
{
    std::string s;
    if (!(is >> s))
        return is;
    try {
        addr = INet4Network(s);
    }
    catch (AddressException &) {
        is.setstate(std::ios::failbit);
    }
    return is;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "INet4Address.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
