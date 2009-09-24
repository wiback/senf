// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Philipp Batroff <pug@berlios.de>
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

// Definition of non-inline non-template functions

// Custom includes
#include <boost/io/ios_state.hpp>
#include <senf/Packets/Packets.hh>
#include "ICMPv6Packet.hh"
#include "ICMPv6TypePacket.hh"

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

#ifndef DOXYGEN

SENF_PACKET_REGISTRY_REGISTER( senf::ICMPTypes, 1,   senf::ICMPv6ErrDestUnreachable );
SENF_PACKET_REGISTRY_REGISTER( senf::ICMPTypes, 2,   senf::ICMPv6ErrTooBig          );
SENF_PACKET_REGISTRY_REGISTER( senf::ICMPTypes, 3,   senf::ICMPv6ErrTimeExceeded    );
SENF_PACKET_REGISTRY_REGISTER( senf::ICMPTypes, 4,   senf::ICMPv6ErrParamProblem    );
SENF_PACKET_REGISTRY_REGISTER( senf::ICMPTypes, 128, senf::ICMPv6EchoRequest        );
SENF_PACKET_REGISTRY_REGISTER( senf::ICMPTypes, 129, senf::ICMPv6EchoReply          );
SENF_PACKET_REGISTRY_REGISTER( senf::ICMPTypes, 130, senf::MLDv2ListenerQuery       );
SENF_PACKET_REGISTRY_REGISTER( senf::ICMPTypes, 143, senf::MLDv2ListenerReport      );

#endif

prefix_ void senf::ICMPv6EchoRequestType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "ICMPv6 Echo Request:\n"
       <<     "  identifier              : " << p->identifier() << "\n"
       <<     "  sequence nr.            : " << p->seqNr() << "\n";
}

///////////////////////////////cc.e////////////////////////////////////////
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
