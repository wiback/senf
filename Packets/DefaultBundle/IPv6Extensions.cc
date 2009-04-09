// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
//     Philipp Batroff <philipp.batroff@fokus.fraunhofer.de>
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

//#include "IPv6Extensions.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    senf::PacketRegistry<senf::IpTypes>::RegistrationProxy<senf::IPv6Extension_Fragment>
        registerIPv6ExtensionType_Fragment (44);
    senf::PacketRegistry<senf::IpTypes>::RegistrationProxy<senf::IPv6Extension_Routing>
        registerIPv6ExtensionType_Routing (43);
    senf::PacketRegistry<senf::IpTypes>::RegistrationProxy<senf::IPv6Extension_HopByHop>
        registerIPv6ExtensionType_HopByHop (0u);
    senf::PacketRegistry<senf::IpTypes>::RegistrationProxy<senf::IPv6Extension_Destination>
        registerIPv6ExtensionType_Destination (60u);
}

prefix_ void senf::IPv6ExtensionType_Fragment::dump(packet p, std::ostream & os)
{
    os << "Internet protocol Version 6 fragment extension:\n"
       << "  next header    : " << unsigned(p->nextHeader()) << "\n"
       << "  fragment offset: " << std::hex << unsigned(p->fragmentOffset()) << "\n"
       << "  more fragments : " << (p->moreFragments()?"yes":"no") << "\n"
       << "  id             : " << std::hex << unsigned(p->id()) << "\n";
}

prefix_ void senf::IPv6ExtensionType_Routing::dump(packet p, std::ostream & os)
{
    os << "Internet protocol Version 6 routing extension:\n"
        << "  next header           : " << unsigned (p->nextHeader()) << "\n"
        << "  header length         : " << unsigned (p->headerLength()) << "\n"
        << "  routing type          : " << unsigned (p->routingType()) << "\n"
        << "  segments left         : " << unsigned (p->segmentsLeft()) << "\n";
        IPv6Extension_Routing::Parser::hopAddresses_t::container hopAddresses (p->hopAddresses());
        os << "  further Hop Addresses : \n";
        if ( p->segmentsLeft() != 0 ){
            for (IPv6Extension_Routing::Parser::hopAddresses_t::container::iterator i (hopAddresses.begin()); i != hopAddresses.end(); ++i)
                os << *i << "\n";
        }
}

prefix_ void senf::IPv6ExtensionType_HopByHop::dump(packet p, std::ostream & os)
{
    os << "Internet protocol Version 6 Hop-By-Hop extension:\n"
        << "  next header           : " << unsigned (p->nextHeader()) << "\n"
        << "  header length         : " << unsigned (p->headerLength()) << "\n";
}

prefix_ void senf::IPv6ExtensionType_Destination::dump(packet p, std::ostream & os)
{
    os << "Internet protocol Version 6 Destination Options extension:\n"
            << "  next header           : " << unsigned (p->nextHeader()) << "\n"
            << "  header length         : " << unsigned (p->headerLength()) << "\n";
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
