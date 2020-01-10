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


#include "TunnelHeaderPacket.hh"

// Custom includes

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

SENF_PACKET_INSTANTIATE_TEMPLATE( senf::emu::TunnelHeaderPacket );

prefix_ void senf::emu::TunnelHeaderPacketType::dump(packet p, std::ostream & os)
{
    os << "TunnelHeader Header: \n"
       << "  reserved  : " << p->reserved() << std::endl
       << "  sequence number : " << p->sequenceNumber() << std::endl;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
