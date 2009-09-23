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
#include <senf/Utils/hexdump.hh>
// Custom includes

//#include "IPv6Extensions.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    senf::PacketRegistry<senf::IpTypes>::RegistrationProxy<senf::IPv6FragmentPacket>
        registerIPv6FragmentPacketType (44);
    senf::PacketRegistry<senf::IpTypes>::RegistrationProxy<senf::IPv6RoutingPacket>
        registerIPv6RoutingPacketType (43);
    senf::PacketRegistry<senf::IpTypes>::RegistrationProxy<senf::IPv6HopByHopOptionsPacket>
        registerIPv6HopByHopOptionsPacketType (0u);
    senf::PacketRegistry<senf::IpTypes>::RegistrationProxy<senf::IPv6DestinationOptionsPacket>
        registerIPv6DestinationOptionsPacketType (60u);
}

prefix_ void senf::IPv6FragmentPacketType::dump(packet p, std::ostream & os)
{
    os << "Internet protocol Version 6 fragment extension:\n"
       <<     "  next header             : " << unsigned(p->nextHeader()) << "\n"
       <<     "  fragment offset         : " << std::hex << unsigned(p->fragmentOffset()) << "\n"
       <<     "  more fragments          : " << (p->moreFragments()?"yes":"no") << "\n"
       <<     "  id                      : " << std::hex << unsigned(p->id()) << "\n";
}

prefix_ void senf::IPv6RoutingPacketType::dump(packet p, std::ostream & os)
{
    os << "Internet protocol Version 6 routing extension:\n"
       <<     "  next header             : " << unsigned (p->nextHeader()) << "\n"
       <<     "  header length           : " << unsigned (p->headerLength()) << "\n"
       <<     "  routing type            : " << unsigned (p->routingType()) << "\n"
       <<     "  segments left           : " << unsigned (p->segmentsLeft()) << "\n"
       <<     "  further Hop Addresses   : \n";
    typedef IPv6RoutingPacket::Parser::hopAddresses_t::container addrContainer_t;
    addrContainer_t hopAddresses (p->hopAddresses());
    if ( p->segmentsLeft() != 0 )
        for (addrContainer_t::iterator i (hopAddresses.begin()); i != hopAddresses.end(); ++i)
            os << *i << "\n";
}

prefix_ void senf::IPv6HopByHopOptionsPacketType::dump(packet p, std::ostream & os)
{
    os << "Internet protocol Version 6 Hop-By-Hop extension:\n"
       <<     "  next header             : " << unsigned (p->nextHeader()) << "\n"
       <<     "  header length           : " << unsigned (p->headerLength()) << "\n";
    os <<     "  OptionTypes:\n";
    typedef IPv6HopByHopOptionsPacket::Parser::options_t::container optContainer_t;
    optContainer_t options (p->options());
    optContainer_t::iterator optIter(options.begin());
    for(; optIter != options.end(); ++optIter) {
        os << "    AltAction             : " << (unsigned) optIter->altAction()
           << "\n    ChangeFlag            : " << (unsigned) optIter->changeFlag()
           << "\n    Option Type           : " << (unsigned) optIter->optionType()
           << "\n    OptionLength          : " << (unsigned)  optIter->optionLength() <<"\n";
        senf::hexdump(boost::begin(optIter->value()) , boost::end(optIter->value()), os );
    }
}

prefix_ void senf::IPv6DestinationOptionsPacketType::dump(packet p, std::ostream & os)
{
    os << "Internet protocol Version 6 Destination Options extension:\n"
       <<     "  next header             : " << unsigned (p->nextHeader()) << "\n"
       <<     "  header length           : " << unsigned (p->headerLength()) << "\n";
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
