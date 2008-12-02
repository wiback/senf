// $Id: main.test.cc 206 2008-06-08 14:20:52Z pug $
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Philipp Batroff <Philipp.Batroff@fokus.fraunhofer.de>
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
#include "../../Packets/Packets.hh"
#include "ICMPv6Packet.hh"
#include "ICMPv6TypePacket.hh"

#ifndef DOXYGEN

namespace {
//Implementing the ICMPv6 Type registry
    senf::PacketRegistry<senf::ICMPTypes>::RegistrationProxy<senf::ICMPV6ErrDestUnreachable>
        registerICMPV6ErrDestUnreachable (1);
    senf::PacketRegistry<senf::ICMPTypes>::RegistrationProxy<senf::ICMPV6ErrTooBig>
        registerICMPV6ErrTooBig (2);
    senf::PacketRegistry<senf::ICMPTypes>::RegistrationProxy<senf::ICMPV6ErrTimeExceeded>
        registerICMPV6ErrTimeExceeded (3);
    senf::PacketRegistry<senf::ICMPTypes>::RegistrationProxy<senf::ICMPV6ErrParamProblem>
        registerICMPV6ErrParamProblem (4);
    senf::PacketRegistry<senf::ICMPTypes>::RegistrationProxy<senf::ICMPv6EchoReq>
        registerICMPv6EchoReq (128);
    senf::PacketRegistry<senf::ICMPTypes>::RegistrationProxy<senf::ICMPv6EchoReply>
        registerICMPv6EchoReply (129);
    senf::PacketRegistry<senf::ICMPTypes>::RegistrationProxy<senf::MLDv2ListenerQuery>
        registerMLDv2ListenerQuery (130);
    senf::PacketRegistry<senf::ICMPTypes>::RegistrationProxy<senf::MLDv2ListenerReport>
        registerMLDv2ListenerReport (143);
}

#endif
