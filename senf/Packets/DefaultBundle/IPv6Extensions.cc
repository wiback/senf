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
    \brief IPv6Extensions non-inline non-template implementation */

#include "IPv6Extensions.hh"

// Custom includes

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_REGISTRY_REGISTER( senf::IPTypes, senf::IPv6FragmentPacketType::ipType,           senf::IPv6FragmentPacket           );
SENF_PACKET_REGISTRY_REGISTER( senf::IPTypes, senf::IPv6RoutingPacketType::ipType,            senf::IPv6RoutingPacket            );
SENF_PACKET_REGISTRY_REGISTER( senf::IPTypes, senf::IPv6HopByHopOptionsPacketType::ipType,    senf::IPv6HopByHopOptionsPacket    );
SENF_PACKET_REGISTRY_REGISTER( senf::IPTypes, senf::IPv6DestinationOptionsPacketType::ipType, senf::IPv6DestinationOptionsPacket );

SENF_PACKET_INSTANTIATE_TEMPLATE( senf::IPv6FragmentPacket           );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::IPv6RoutingPacket            );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::IPv6HopByHopOptionsPacket    );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::IPv6DestinationOptionsPacket );

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::IPv6FragmentPacketType

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

prefix_ void senf::IPv6FragmentPacketType::finalize(packet p)
{
    try {
        p->nextHeader() << key(p.next());
    }
    catch (InvalidPacketChainException & ex) {
        p->nextHeader() << 59; // No next header
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::IPv6RoutingPacketType

prefix_ void senf::IPv6RoutingPacketType::dump(packet p, std::ostream & os)
{
    os << "Internet protocol Version 6 routing extension:\n"
       << senf::fieldName("next header")               << unsigned(p->nextHeader()) << "\n"
       << senf::fieldName("header length")             << unsigned(p->headerLength()) << "\n"
       << senf::fieldName("routing type")              << unsigned(p->routingType()) << "\n"
       << senf::fieldName("segments left")             << unsigned(p->segmentsLeft()) << "\n"
       << "  further Hop Addresses:\n";
    typedef IPv6RoutingPacket::Parser::hopAddresses_t::container_type addrContainer_t;
    addrContainer_t hopAddresses (p->hopAddresses());
    if ( p->segmentsLeft() != 0 )
        for (addrContainer_t::iterator i (hopAddresses.begin()); i != hopAddresses.end(); ++i)
            os << "    " << *i << "\n";
}

prefix_ void senf::IPv6RoutingPacketType::finalize(packet p)
{
    try {
        p->nextHeader() << key(p.next());
    }
    catch (InvalidPacketChainException & ex) {
        p->nextHeader() << 59; // No next header
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::IPv6HopByHopOptionsPacketType

prefix_ void senf::IPv6HopByHopOptionsPacketType::dump(packet p, std::ostream & os)
{
    os << "Internet protocol Version 6 Hop-By-Hop extension:\n"
       << senf::fieldName("next header")               << unsigned(p->nextHeader()) << "\n"
       << senf::fieldName("header length")             << unsigned(p->headerLength()) <<" ("<< unsigned((1+p->headerLength())*8)<< ")\n";
    os << "  OptionTypes:\n";
    typedef IPv6HopByHopOptionsPacket::Parser::options_t::container_type optContainer_t;
    optContainer_t options (p->options());
    for (optContainer_t::const_iterator i = options.begin(); i != options.end(); ++i)
        i->dump( os);
}

prefix_ void senf::IPv6HopByHopOptionsPacketType::finalize(packet p)
{
    try {
        p->nextHeader() << key(p.next());
    }
    catch (InvalidPacketChainException & ex) {
        p->nextHeader() << 59; // No next header
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::EthernetPacketType

prefix_ void senf::IPv6DestinationOptionsPacketType::dump(packet p, std::ostream & os)
{
    os << "Internet protocol Version 6 Destination Options extension:\n"
       << senf::fieldName("next header")               << unsigned (p->nextHeader()) << "\n"
       << senf::fieldName("header length")             << unsigned (p->headerLength()) << "\n";
    os << "  OptionTypes:\n";
    typedef IPv6DestinationOptionsPacket::Parser::options_t::container_type optContainer_t;
    optContainer_t options (p->options());
    for (optContainer_t::const_iterator i = options.begin(); i != options.end(); ++i)
        i->dump( os);
}

prefix_ void senf::IPv6DestinationOptionsPacketType::finalize(packet p)
{
    try {
        p->nextHeader() << key(p.next());
    }
    catch (InvalidPacketChainException & ex) {
        p->nextHeader() << 59; // No next header
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
