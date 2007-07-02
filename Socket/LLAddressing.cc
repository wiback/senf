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
    \brief LLSocketAddress and LLAddressingPolicy non-inline non-template implementation
 */

#include "LLAddressing.hh"
#include "LLAddressing.ih"

// Custom includes
#include <net/if.h>
#include <sys/socket.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/finder.hpp>

#include "Utils/Exception.hh"

//#include "LLAddressing.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ unsigned char senf::detail::hexnibble(char c)
{
    if (c>='0' && c<='9')
        return c - '0';
    if (c>='A' && c<='F')
        return c - 'A' + 10;
    if (c>='a' && c<='f')
        return c - 'a' + 10;
    throw InvalidLLSocketAddressException();
}

prefix_ std::string senf::LLSocketAddress::interface()
    const
{
    if (addr_.sll_ifindex == 0)
        return std::string();
    char name[IFNAMSIZ];
    if (! ::if_indextoname(addr_.sll_ifindex, name))
        throw InvalidLLSocketAddressException();
    return std::string(name);
}

/*
{
    if (addr_.sll_halen == 0)
        return std::string();
    std::stringstream s;

    unsigned char const * i = &addr_.sll_addr[0];
    while (1) {
        s << std::hex << std::setw(2) << std::setfill('0') << unsigned(*i);
        ++i;
        if (i == &addr_.sll_addr[addr_.sll_halen]) break;
        s << '-';
    }
    return s.str();
}
*/


/*
prefix_ void senf::LLSocketAddress::address(std::string address)
{
    typedef boost::split_iterator<std::string::iterator> StringSplitIterator;
    StringSplitIterator i = boost::make_split_iterator(address, boost::token_finder(boost::is_any_of("-: ")));
    unsigned char * j = &addr_.sll_addr[0];
    for (; ! i.eof() && addr_.sll_halen<8; ++i, ++j, ++addr_.sll_halen) {
        if ( i->size() != 2 || ! boost::all(*i, boost::is_xdigit()) )
            throw InvalidLLSocketAddressException();
        *j = hex(*i);
    }
    if (! i.eof())
        throw InvalidLLSocketAddressException();
}
*/

prefix_ void senf::LLSocketAddress::interface(std::string interface)
{
    if (! interface.empty()) {
        addr_.sll_ifindex = if_nametoindex(interface.c_str());
        if (addr_.sll_ifindex == 0)
            throw InvalidLLSocketAddressException();
    }
}


prefix_ senf::detail::LLAddressFromStringRange
senf::llAddress(std::string address)
{
    detail::StringSplitIterator i =
        boost::make_split_iterator(address, boost::token_finder(boost::is_any_of("-: ")));
    detail::StringSplitIterator i_end;

    detail::HexSplitIterator j (i,detail::HexConverter());
    detail::HexSplitIterator j_end (i_end);

    return detail::LLAddressFromStringRange(j,j_end);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "LLAddressing.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
