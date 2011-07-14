// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on, 
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis, 
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Stefan Bund <g0dil@berlios.de>

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

#include <senf/Socket/Protocols/AddressExceptions.hh>

//#include "LLAddressing.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ std::string senf::LLSocketAddress::interface()
    const
{
    if (addr_.sll_ifindex == 0)
        return std::string();
    char name[IFNAMSIZ];
    ::bzero(name, IFNAMSIZ);
    if (! ::if_indextoname(addr_.sll_ifindex, name))
        throw UnknownInterfaceException();
    return std::string(name);
}

prefix_ void senf::LLSocketAddress::interface(std::string const & iface)
{
    if (iface.empty())
        addr_.sll_ifindex = 0;
    else {
        addr_.sll_ifindex = if_nametoindex(iface.c_str());
        if (addr_.sll_ifindex == 0)
            throw UnknownInterfaceException(iface);
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
