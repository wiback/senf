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
    \brief GREPacket non-inline non-template implementation */

#include "GREPacket.hh"
//#include "GREPacket.ih"

// Custom includes
#include <boost/io/ios_state.hpp>
#include <iomanip>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_PACKET_REGISTRY_REGISTER( senf::IPTypes, senf::GREPacketType::ipType, senf::GREPacket );

prefix_ void senf::GREPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "GRE Encapsulation:\n"
       << senf::fieldName("checksum_present")          << p->checksum_present() << "\n"
       << senf::fieldName("protocol_type")             << "0x" << std::hex << p->protocol_type() << "\n";
}

prefix_ void senf::GREPacketType::finalize(packet p)
{
    p->protocol_type() << key(p.next(nothrow));
    p->version_number() = 0; // as per RFC2784, 2.3.1
    if (p->checksum_present()) {
        // compute checksum
    } else {
        // ???
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
