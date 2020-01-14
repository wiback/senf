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
