//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


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
    auto const & _tmp_to_make_clang_happy_ (protocol().policy());
    map["socket.protocol.policy"] << prettyName(typeid(_tmp_to_make_clang_happy_));
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
