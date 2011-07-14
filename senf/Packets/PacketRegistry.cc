// $Id$
//
// Copyright (C) 2007
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
