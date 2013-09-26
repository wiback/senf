// $Id:TransportPacket.hh 560 2007-12-13 14:39:37Z tho $
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
//   David Wagner <dw6@berlios.de>
/** \file
    \brief DTCPPacket non-inline non-template implementation */

#include "DTCPPacket.hh"
//#include "DTCPPacket.ih"

// Custom includes
#include <boost/io/ios_state.hpp>
#include <iomanip>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ void senf::DTCPHelloPacketType::dump(packet p, std::ostream & os)
{
    static char const * COMMANDS[] = { "0", "JOIN", "LEAVE", "3", "4", "5", "6", "7",
                                       "8", "9", "10", "11", "12", "13", "14", "15" };
    boost::io::ios_all_saver ias(os);
    os << "DTCP HELLO Packet:" << std::endl
       << senf::fieldName("version")                   << p->versionNumber()                       << "\n"
       << senf::fieldName("command")                   << COMMANDS[p->command()]                   << "\n"
       << senf::fieldName("interval")                  << unsigned(p->interval())                  << "\n"
       << senf::fieldName("sequence number")           << p->sequenceNumber()                      << "\n"
       << senf::fieldName("receive capable feed")      << (p->receiveCapableFeed() ? "yes" : "no") << "\n"
       << senf::fieldName("ip version")                << p->ipVersion()                           << "\n"
       << senf::fieldName("tunnel protocol")           << unsigned(p->tunnelProtocol())            << "\n"
       << senf::fieldName("number of BDL ips")         << unsigned(p->fbipCount())                 << "\n"
       << "  feed BDL ips:\n";

    switch (p->ipVersion()) {
    case 4 : {
        typedef DTCPHelloPacket::Parser::v4fbipList_t FBIPList;
        FBIPList::container_type fbips (p->v4fbipList());
        FBIPList::container_type::iterator i (fbips.begin());
        FBIPList::container_type::iterator const i_end (fbips.end());
        for (; i != i_end; ++i)
            os << "    " << *i << "\n";
        break;
    }
    case 6 : {
        typedef DTCPHelloPacket::Parser::v6fbipList_t FBIPList;
        FBIPList::container_type fbips (p->v6fbipList());
        FBIPList::container_type::iterator i (fbips.begin());
        FBIPList::container_type::iterator const i_end (fbips.end());
        for (; i != i_end; ++i)
            os << "    " << *i << "\n";
        break;
    }
    default:
        os << "    unknown ip version\n";
    }
}

#undef prefix_


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
