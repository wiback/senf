//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
