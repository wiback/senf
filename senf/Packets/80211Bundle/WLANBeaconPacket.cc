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


#include "WLANBeaconPacket.hh"

// Custom includes
#include <boost/io/ios_state.hpp>
#include "Registries.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_REGISTRY_REGISTER( senf::WLANManagementSubtypes, 8u, senf::WLANBeaconPacket );
SENF_PACKET_INSTANTIATE_TEMPLATE( senf::WLANBeaconPacket );

prefix_ void senf::WLANBeaconPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "WLAN beacon frame:\n"
       << senf::fieldName("timestamp")      << unsigned( p->timestamp())      << "\n"
       << senf::fieldName("beaconInterval") << unsigned( p->beaconInterval()) << "\n";
    p->ssidIE().dump( os);
    p->supportedRatesIE().dump( os);
    os << "  Optional Information Elements:\n";
    typedef parser::ieList_t::container_type ieListContainer_t;
    ieListContainer_t ieListContainer (p->ieList());
    for (ieListContainer_t::const_iterator i = ieListContainer.begin(); i != ieListContainer.end(); ++i)
        i->dump( os);
}


SENF_PACKET_INSTANTIATE_TEMPLATE( senf::InformationElementsPacket );

prefix_ void senf::InformationElementsPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "Information Elements:\n";
    typedef parser::ieList_t::container_type ieListContainer_t;
    ieListContainer_t ieListContainer (p->ieList());
    for (ieListContainer_t::const_iterator i = ieListContainer.begin(); i != ieListContainer.end(); ++i)
        i->dump( os);
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
