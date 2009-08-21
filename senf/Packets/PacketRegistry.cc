// $Id$
//
// Copyright (C) 2007
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
    \brief PacketRegistry non-inline non-template implementation */


// Custom includes
#include "Packets.hh"

//#include "PacketRegistry.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_  senf::PkReg_Entry::~PkReg_Entry()
{}

prefix_ senf::detail::PacketRegistryImplBase::~PacketRegistryImplBase()
{}

prefix_ void senf::detail::PacketRegistryImplBase::dump(std::ostream & os)
{
    RegistryMap::const_iterator i (registries().begin());
    RegistryMap::const_iterator const i_end (registries().end());
    for (; i!=i_end; ++i) {
        os << i->first << ":\n";
        i->second->v_dump(os);
        os << "\n";
    }
}

prefix_ senf::detail::PacketRegistryImplBase::RegistryMap &
senf::detail::PacketRegistryImplBase::registries()
{
    static RegistryMap map;
    return map;
}

///////////////////////////////cc.e////////////////////////////////////////
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
