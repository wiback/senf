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


#include "MGENPacket.hh"

// Custom includes
#include <iomanip>

#define prefix_

prefix_ void MGENPacketType::dump(packet p, std::ostream & os)
{
    os << "MGENPacket\n"
       << "  messageSize:        " << unsigned(p->messageSize()) << "\n"
       << "  version:            " << unsigned(p->version()) << "\n"
       << "  flags:              " << unsigned(p->flags()) << "\n"
       << "  flowId:             " << unsigned(p->flowId()) << "\n"
       << "  sequenceNumber:     " << unsigned(p->sequenceNumber()) << "\n"
       << "  txTimeSeconds:      " << unsigned(p->txTimeSeconds()) << "\n"
       << "  txTimeMicroseconds: " << unsigned(p->txTimeMicroseconds()) << "\n"
       << "  dstPort:            " << unsigned(p->dstPort()) << "\n"
       << "  dstAddrType:        " << unsigned(p->dstAddrType()) << "\n"
       << "  dstAddrLen:         " << unsigned(p->dstAddrLen()) << "\n"
       << "  dstAddr:            ";
    switch (p->dstAddrType()) {
    case 0:
        os << "n/a" << "\n";
        break;
    case 1:
        os << p->dstAddrV4() << "\n";
        break;
    case 2:
        os << p->dstAddrV6() << "\n";
        break;
    }
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
