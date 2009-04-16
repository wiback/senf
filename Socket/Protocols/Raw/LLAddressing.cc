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
    \brief LLSocketAddress and LLAddressingPolicy non-inline non-template implementation
 */

#include "LLAddressing.hh"
//#include "LLAddressing.ih"

// Custom includes
#include <net/if.h>
#include <sys/socket.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/finder.hpp>

#include "../../../Utils/Exception.hh"
#include "../AddressExceptions.hh"

//#include "LLAddressing.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ std::string senf::LLSocketAddress::interface()
    const
{
    if (addr_.sll_ifindex == 0)
        return std::string();
    char name[IFNAMSIZ];
    ::bzero(name, IFNAMSIZ);
    if (! ::if_indextoname(addr_.sll_ifindex, name))
        throw AddressSyntaxException();
    return std::string(name);
}

prefix_ void senf::LLSocketAddress::interface(std::string const & iface)
{
    if (iface.empty()) 
        addr_.sll_ifindex = 0;
    else {
        addr_.sll_ifindex = if_nametoindex(iface.c_str());
        if (addr_.sll_ifindex == 0)
            throw AddressSyntaxException();
    }
}

prefix_ std::ostream & senf::operator<<(std::ostream & os, LLSocketAddress const & llAddr)
{
    os << "[" << llAddr.address()
       << '%' << llAddr.interface() 
       << ' ' << llAddr.protocol()
       << ' ' << llAddr.arptype()
       << ( llAddr.pkttype() == senf::LLSocketAddress::Host ? " Host" :
            llAddr.pkttype() == senf::LLSocketAddress::Broadcast ? " Broadcast" :
            llAddr.pkttype() == senf::LLSocketAddress::Multicast ? " Multicast" :
            llAddr.pkttype() == senf::LLSocketAddress::OtherHost ? " OtherHost" :
            llAddr.pkttype() == senf::LLSocketAddress::Outgoing ? " Outgoing" :
            llAddr.pkttype() == senf::LLSocketAddress::Broadcast ? "Broadcast" : "" )
       << "]";
    return os;
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
// comment-column: 40
// End:
