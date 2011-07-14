// $Id$
//
// Copyright (C) 2006
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
    \brief SocketHandle non-inline non-template implementation
 */

#include "SocketHandle.hh"
#include "SocketHandle.ih"

// Custom includes
#include <sstream>
#include <senf/Utils/TypeInfo.hh>

//#include "SocketHandle.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::SocketBody

prefix_ void senf::SocketBody::state(SocketStateMap & map, unsigned lod)
{
    map["file.handle"]     << fd();
    map["file.refcount"]   << refcount();
    map["socket.server"]   << isServer();
    map["socket.protocol"] << v_protocolName();
    map["socket.protocol.policy"] << prettyName(typeid(protocol().policy()));
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
