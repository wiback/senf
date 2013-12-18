// $Id$
//
// Copyright (C) 2007
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
//   Joachim Kaeber <jkaeber@berlios.de>
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
