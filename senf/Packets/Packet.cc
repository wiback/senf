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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief Packet non-inline non-template implementation */

//#include "Packet.ih"

// Custom includes
#include "Packets.hh"

//#include "Packet.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::Packet senf::Packet::getNext(PacketInterpreterBase::optional_range const & range)
    const
{
    factory_t factory (ptr()->nextPacketType());
    if (factory)
        return parseNextAs(factory, range);
    else
        return parseNextAs<DataPacket>();
}

prefix_ senf::Packet senf::Packet::getLast()
    const
{
    Packet p (*this);
    Packet n (p.next(nothrow));
    while (n) {
        p = n;
        n = p.next(nothrow);
    }
    return p;
}

prefix_ void senf::Packet::dump(std::ostream & os)
    const
{
    last(); // Make sure the packet is complete
    ptr()->dump(os);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "Packet.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
