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


#include "IperfPacket.hh"

// Custom includes

#define prefix_

prefix_ void IperfUDPPacketType::dump(packet p, std::ostream & os)
{
    os << "Iperf UDP Packet\n"
       << "  id:      " << p->id() << std::endl
       << "  tv_sec:  " << p->tv_sec() << std::endl
       << "  tv_usec: " << p->tv_usec() << std::endl;
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
