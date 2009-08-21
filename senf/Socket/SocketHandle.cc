// $Id$
//
// Copyright (C) 2006
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
    \brief SocketHandle non-inline non-template implementation
 */

#include "SocketHandle.hh"
#include "SocketHandle.ih"

// Custom includes
#include <sstream>
#include <sys/socket.h>
#include "../Utils/TypeInfo.hh"

//#include "SocketHandle.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::SocketBody

prefix_ void senf::SocketBody::state(SocketStateMap & map, unsigned lod)
{
    map["file.handle"]     << fd();
    map["file.refcount"]   << refcount();
    map["socket.server"]   << isServer();
    map["socket.protocol"] << v_protocolName();
    map["socket.protocol.policy"]   << prettyName(typeid(protocol().policy()));
    protocol().state(map,lod);
}

prefix_ void senf::SocketBody::v_close()
{
    protocol().close();
}

prefix_ void senf::SocketBody::v_terminate()
{
    protocol().terminate();
}

prefix_ bool senf::SocketBody::v_eof()
    const
{
    return protocol().eof();
}

///////////////////////////////////////////////////////////////////////////
// senf::detail::StateMapOrdering

namespace {
    bool contains(std::string::const_iterator b, std::string::const_iterator e, char c)
    {
        for (; b != e; ++b)
            if (*b == c)
                return true;
        return false;
    }
}

prefix_ std::string senf::detail::dumpState(SocketStateMap const & map)
{
    std::stringstream s;
    SocketStateMap::const_iterator i (map.begin());
    SocketStateMap::const_iterator i_end (map.end());
    for (; i != i_end; ++i)
        s << i->first << ": " << i->second << "\n";
    return s.str();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "SocketHandle.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
