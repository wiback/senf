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
    \brief MIH Message-Registry non-inline non-template implementation */

#include "MIHMessageRegistry.hh"

// Custom includes

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::MIHMessageRegistry

prefix_ void senf::MIHMessageRegistry::validate(key_t messageId, senf::Packet message)
{
    Map::const_iterator i (map_.find( messageId));
    if (i != map_.end())
        i->second->validate( message);
    else
        throw InvalidMIHPacketException("unknown message id: ") << messageId;
}

prefix_ senf::MIHMessageRegistry::const_iterator senf::MIHMessageRegistry::begin()
    const
{
    return map_.begin();
}

prefix_ senf::MIHMessageRegistry::const_iterator senf::MIHMessageRegistry::end()
    const
{
    return map_.end();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
