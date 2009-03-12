// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     David Wagner <dw6@berlios.de>
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
    \brief UNAddressing non-inline non-template implementation */

#include "UNAddressing.hh"
//#include "UNAddressing.ih"

// Custom includes
#include "../AddressExceptions.hh"

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ void senf::UNSocketAddress::path(std::string const & path)
{
    if (path.size() > sizeof(sockaddr_un)-sizeof(short)-1)
        throw AddressSyntaxException() << "UNSocketAddress path too long: " << path;
    socklen(path.size()+sizeof(short));
    memcpy(addr_.sun_path, path.c_str(), socklen()-sizeof(short));
    addr_.sun_path[socklen()-sizeof(short)+1] = 0;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "UNAddressing.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
