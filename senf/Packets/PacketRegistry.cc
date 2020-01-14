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
    \brief PacketRegistry non-inline non-template implementation */


// Custom includes
#include "Packets.hh"

//#include "PacketRegistry.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::detail::PacketRegistryImplBase::~PacketRegistryImplBase()
{}

prefix_ void senf::detail::PacketRegistryImplBase::dump(std::ostream & os)
{
    RegistryMap::const_iterator i (registries().begin());
    RegistryMap::const_iterator const i_end (registries().end());
    for (; i!=i_end; ++i) {
        if (! i->second->v_empty()) {
            os << i->first << ":\n";
            i->second->v_dump(os);
            os << "\n";
        }
    }
}

prefix_ void senf::detail::PacketRegistryImplBase::clear()
{
    RegistryMap::const_iterator i (registries().begin());
    RegistryMap::const_iterator const i_end (registries().end());
    for (; i!=i_end; ++i)
        i->second->v_clear();
}

prefix_ senf::detail::PacketRegistryImplBase::RegistryMap &
senf::detail::PacketRegistryImplBase::registries()
{
    static RegistryMap map;
    return map;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "PacketRegistry.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
