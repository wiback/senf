// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
//     Philipp Batroff <philipp.batroff@fokus.fraunhofer.de>
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
    \brief IPv6Extensions non-inline non-template implementation */

#include "IPv6Extensions.hh"
//#include "IPv6Extensions.ih"

// Custom includes
#include <senf/Utils/hexdump.hh>

//#include "IPv6Extensions.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    SENF_PACKET_REGISTRY_REGISTER( senf::IpTypes, 44,  senf::IPv6FragmentPacket           );
    SENF_PACKET_REGISTRY_REGISTER( senf::IpTypes, 43,  senf::IPv6RoutingPacket            );
    SENF_PACKET_REGISTRY_REGISTER( senf::IpTypes, 0u,  senf::IPv6HopByHopOptionsPacket    );
    SENF_PACKET_REGISTRY_REGISTER( senf::IpTypes, 60u, senf::IPv6DestinationOptionsPacket );
}

prefix_ void senf::IPv6FragmentPacketType::dump(packet p, std::ostream & os)
{
    os << "Internet protocol Version 6 fragment extension:\n"
       << senf::fieldName("next header")               << unsigned(p->nextHeader()) << "\n"
       << senf::fieldName("fragment offset")
       << "0x" << std::hex << unsigned(p->fragmentOffset()) << "\n"
       << senf::fieldName("more fragments")            << (p->moreFragments()?"yes":"no") << "\n"
       << senf::fieldName("id")
       << "0x" << std::hex << unsigned(p->id()) << "\n";
}

prefix_ void senf::IPv6RoutingPacketType::dump(packet p, std::ostream & os)
{
    os << "Internet protocol Version 6 routing extension:\n"
       << senf::fieldName("next header")               << unsigned(p->nextHeader()) << "\n"
       << senf::fieldName("header length")             << unsigned(p->headerLength()) << "\n"
       << senf::fieldName("routing type")              << unsigned(p->routingType()) << "\n"
       << senf::fieldName("segments left")             << unsigned(p->segmentsLeft()) << "\n"
       << "  further Hop Addresses:\n";
    typedef IPv6RoutingPacket::Parser::hopAddresses_t::container addrContainer_t;
    addrContainer_t hopAddresses (p->hopAddresses());
    if ( p->segmentsLeft() != 0 )
        for (addrContainer_t::iterator i (hopAddresses.begin()); i != hopAddresses.end(); ++i)
            os << "    " << *i << "\n";
}

prefix_ void senf::IPv6HopByHopOptionsPacketType::dump(packet p, std::ostream & os)
{
    os << "Internet protocol Version 6 Hop-By-Hop extension:\n"
       << senf::fieldName("next header")               << unsigned(p->nextHeader()) << "\n"
       << senf::fieldName("header length")             << unsigned(p->headerLength()) << "\n";
    os << "  OptionTypes:\n";
    typedef IPv6HopByHopOptionsPacket::Parser::options_t::container optContainer_t;
    optContainer_t options (p->options());
    for (optContainer_t::const_iterator i = options.begin(); i != options.end(); ++i)
        i->dump( os);
}

prefix_ void senf::IPv6DestinationOptionsPacketType::dump(packet p, std::ostream & os)
{
    os << "Internet protocol Version 6 Destination Options extension:\n"
       << senf::fieldName("next header")               << unsigned (p->nextHeader()) << "\n"
       << senf::fieldName("header length")             << unsigned (p->headerLength()) << "\n";
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "IPv6Extensions.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
