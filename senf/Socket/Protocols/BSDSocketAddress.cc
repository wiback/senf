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
    \brief BSDSocketAddress non-inline non-template implementation */

#include "BSDSocketAddress.hh"
//#include "BSDSocketAddress.ih"

// Custom includes
#include <senf/Socket/Protocols/INet/INetAddressing.hh>
#include <senf/Socket/Protocols/UN/UNAddressing.hh>
#include <senf/Socket/Protocols/Raw/LLAddressing.hh>

//#include "BSDSocketAddress.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ std::ostream & senf::operator<<(std::ostream & os, BSDSocketAddress const & addr)
{
    switch(addr.family()) {
    case INet4SocketAddress::addressFamily :
        os << sockaddr_cast<INet4SocketAddress>(addr);
        break;
    case INet6SocketAddress::addressFamily :
        os << sockaddr_cast<INet6SocketAddress>(addr);
        break;
    case UNSocketAddress::addressFamily :
        os << sockaddr_cast<UNSocketAddress>(addr);
        break;
    case LLSocketAddress::addressFamily :
        os << sockaddr_cast<LLSocketAddress>(addr);
        break;
    case AF_UNSPEC :
        os << "[unspecified address]";
        break;
    default:
        os << "[unknown address family " << addr.family() << "]";
        break;
    }
    return os;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "BSDSocketAddress.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
