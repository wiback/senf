// $Id:TransportPacket.hh 560 2007-12-13 14:39:37Z tho $
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     David Wagner <dw6@berlios.de>
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
    \brief DTCPPacket non-inline non-template implementation */

//#include "DTCPPacket.hh"
//#include "DTCPPacket.ih"

// Custom includes
#include "DTCPPacket.hh"
#include <boost/io/ios_state.hpp>
#include <iomanip>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ void senf::DTCPHelloPacketType::dump(packet p, std::ostream & os)
{
    static char const * COMMANDS[] = { "0", "JOIN", "LEAVE", "3", "4", "5", "6", "7",
                                       "8", "9", "10", "11", "12", "13", "14", "15" };
    boost::io::ios_all_saver ias(os);
    os << "DTCP HELLO Packet:" << std::endl
       <<     "  version                 : " << p->versionNumber()                       << std::endl
       <<     "  command                 : " << COMMANDS[p->command()]                   << std::endl
       <<     "  interval                : " << unsigned(p->interval())                  << std::endl
       <<     "  sequence number         : " << p->sequenceNumber()                      << std::endl
       <<     "  receive capable feed    : " << (p->receiveCapableFeed() ? "yes" : "no") << std::endl
       <<     "  ip version              : " << p->ipVersion()                           << std::endl
       <<     "  tunnel protocol         : " << unsigned(p->tunnelProtocol())            << std::endl
       <<     "  number of BDL ips       : " << unsigned(p->fbipCount())                 << std::endl
       <<     "  feed BDL ips            : ";
    
    switch (p->ipVersion()) {
    case 4 : {
        typedef DTCPHelloPacket::Parser::v4fbipList_t FBIPList;
        FBIPList::container fbips (p->v4fbipList());
        FBIPList::container::iterator i (fbips.begin());
        FBIPList::container::iterator const i_end (fbips.end());
        for (; i != i_end; ++i)
            os << "\n    " << *i;
        break;
    }
    case 6 : {
        typedef DTCPHelloPacket::Parser::v6fbipList_t FBIPList;
        FBIPList::container fbips (p->v6fbipList());
        FBIPList::container::iterator i (fbips.begin());
        FBIPList::container::iterator const i_end (fbips.end());
        for (; i != i_end; ++i)
            os << "\n    " << *i;
        break;
    }
    default:
        os << "unknown ip version";
    }
    
    os << std::endl;
}

#undef prefix_


///////////////////////////////cc.e////////////////////////////////////////
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
