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

// Definition of non-inline non-template functions

#include "SocketHandle.hh"
#include "SocketHandle.ih"

// Custom includes
#include <sstream>
#include <sys/socket.h>
#include "Utils/TypeInfo.hh"

//#include "SocketHandle.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ void satcom::lib::SocketBody::v_close()
{
    if (::shutdown(fd(),SHUT_RDWR) < 0)
        throw SystemException(errno);
    if (::close(fd()) < 0)
        throw SystemException(errno);
}

prefix_ void satcom::lib::SocketBody::v_terminate()
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

prefix_ bool satcom::lib::SocketBody::v_eof()
    const
{
    return protocol().eof();
}

prefix_ void satcom::lib::SocketBody::state(SocketStateMap & map, unsigned lod)
{
    map["file.handle"] = fd();
    map["file.refcount"] = refcount();
    map["socket.server"] = isServer();
    map["socket.protocol"] = prettyName(typeid(protocol()));
    map["socket.policy"] = prettyName(typeid(protocol().policy()));
    protocol().state(map,lod);
}

///////////////////////////////////////////////////////////////////////////
// satcom::lib::detail::StateMapOrdering

namespace {
    bool contains(std::string::iterator b, std::string::iterator e, char c)
    {
        for (; b != e; ++b)
            if (*b == c)
                return true;
        return false;
    }
}

prefix_ bool satcom::lib::detail::StateMapOrdering::operator()(std::string a1, std::string a2)
    const
{
    std::string::iterator i1 (a1.begin());
    std::string::iterator const i1_end (a1.end());
    std::string::iterator i2 (a2.begin());
    std::string::iterator const i2_end (a2.end());
    for(; i1 != i1_end && i2 != i2_end && *i1 == *i2; ++i1, ++i2) ;
    if (i1 == i1_end) {
        if (i2 == i2_end)
            // the strings are equal
            return false;
        if (contains(i2,i2_end,'.'))
            // the longer string is a sub-'directory' of the shorter
            return true;
        return *i1 < *i2;
    }
    else if (i2 == i2_end) { // && i1 != i1_end
        if (contains(i1,i1_end,'.'))
            // the longer string is a sub-'directory' of the shorter
            return false;
        return *i1 < *i2;
    }
    if (contains(i1,i1_end,'.')) {
        if (contains(i2,i2_end,'.'))
            return *i1 < *i2;
        return false;
    }
    else if (contains(i2,i2_end,'.'))
        return true;
    return *i1 < *i2;
}

prefix_ std::string satcom::lib::detail::dumpState(SocketStateMap const & map)
{
    std::stringstream s;
    SocketStateMap::const_iterator i (map.begin());
    SocketStateMap::const_iterator i_end (map.end());
    for (; i != i_end; ++i)
        s << i->first << ": " << i->second << "\n";
    return s.str();
}

template <class Policy>
prefix_ std::ostream & satcom::lib::operator<<(std::ostream & os, SocketHandle<Policy> handle)
{
    os << handle.dumpState();
    return os;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "SocketHandle.mpp"


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
