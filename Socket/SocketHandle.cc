// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

prefix_ void senf::SocketBody::v_close()
{
    if (::shutdown(fd(),SHUT_RDWR) < 0)
        throw SystemException(errno);
    if (::close(fd()) < 0)
        throw SystemException(errno);
}

prefix_ void senf::SocketBody::v_terminate()
{
    struct linger ling;
    ling.l_onoff = 0;
    ling.l_linger = 0;

    // We purposely IGNORE any errors: this method is used to try and
    // terminate the connection ignoring any possible problems

    ::setsockopt(fd(),SOL_SOCKET,SO_LINGER,&ling,sizeof(ling));
    ::shutdown(fd(),SHUT_RDWR);
    ::close(fd());
}

prefix_ bool senf::SocketBody::v_eof()
    const
{
    return protocol().eof();
}

prefix_ void senf::SocketBody::state(SocketStateMap & map, unsigned lod)
{
    map["file.handle"] = fd();
    map["file.refcount"] = refcount();
    map["socket.server"] = isServer();
    map["socket.protocol"] = prettyName(typeid(protocol()));
    map["socket.policy"] = prettyName(typeid(protocol().policy()));
    protocol().state(map,lod);
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

template <class Policy>
prefix_ std::ostream & senf::operator<<(std::ostream & os, SocketHandle<Policy> handle)
{
    os << handle.dumpState();
    return os;
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
