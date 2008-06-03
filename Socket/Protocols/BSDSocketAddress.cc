// $Id$
//
// Copyright (C) 2008 
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
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
    \brief BSDSocketAddress non-inline non-template implementation */

//#include "BSDSocketAddress.hh"
//#include "BSDSocketAddress.ih"

// Custom includes
#include "INet/INetAddressing.hh"
#include "UN/UNAddressing.hh"
#include "Raw/LLAddressing.hh"

//#include "BSDSocketAddress.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

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

///////////////////////////////cc.e////////////////////////////////////////
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
