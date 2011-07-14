// $Id$
//
// Copyright (C) 2008
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
