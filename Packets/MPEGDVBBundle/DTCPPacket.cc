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


#include "DTCPPacket.hh"
#include <boost/io/ios_state.hpp>
#include <iomanip>

#define prefix_

prefix_ void senf::DTCPPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "DTCPPacket" << std::endl
       << "  version              : " << p->version_number()              << std::endl
       << "  command              : " << p->command()                     << std::endl
       << "  interval             : " << p->interval()                    << std::endl
       << "  sequence_number      : " << p->sequence_number()             << std::endl
       << "  receive_capable_feed : " << p->receive_capable_feed()        << std::endl
       << "  ip_version           : " << p->ip_version()                  << std::endl
       << "  tunnel_protocol      : " << p->tunnel_protocol()             << std::endl
       ;
       
        //TODO: print included IPs
}

#undef prefix_
