// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief MIH protocol non-inline non-template implementation */

#include "MIHPacket.hh"
//#include "MIHPacket.ih"

// Custom includes
#include "../../Packets/Packets.hh"
#include <boost/io/ios_state.hpp>

#define prefix_


prefix_ void senf::MIHPacketType::dump(packet p, std::ostream &os)
{
    boost::io::ios_all_saver ias(os);
    os << "MIH Packet:\n"
       << "  protocol header:\n"
       << "    Version:         " << unsigned( p->version()) << "\n"
       << "    Ack-Request:     " << p->ackRequest() << "\n"
       << "    Ack-Response:    " << p->ackResponse() << "\n"
       << "    UIR:             " << p->uir() << "\n"
       << "    more fragment:   " << p->moreFragment() << "\n"
       << "    fragment number: " << p->fragmentNr() << "\n"
       << "    message ID (MID):\n"
       << "      SID:           " << unsigned( p->sid()) << "\n"        
       << "      Opcode:        " << unsigned( p->opcode()) << "\n"
       << "      AID:           " << unsigned( p->aid()) << "\n"      
       << "    Transaction ID:  " << unsigned( p->transactionId()) << "\n"
       << "    payload length:  " << unsigned( p->payloadLength()) << "\n";
}


prefix_ void senf::MIHPacketType::finalize(packet p)
{
    p->source_length() << senf::bytes( p->source_mihf_id());
    p->destination_length() << senf::bytes( p->destination_mihf_id());
    p->payloadLength_() << p.size() - 8;
}


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
