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
    \brief main.test public header */

#ifndef HH_SENF_Packets_main_test_
#define HH_SENF_PAckets_main_test_ 1

// Custom includes

//#include "main.test.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace detail {
namespace packet {
namespace test {

    struct TestDriver {
        template <class T>
        static typename PacketInterpreter<T>::ptr create(PacketImpl * impl, iterator b, iterator e,
                                                         PacketInterpreterBase::Append_t)
            { return PacketInterpreter<T>::create(impl,b,e,PacketInterpreterBase::Append); }

        template <class T>
        static typename PacketInterpreter<T>::ptr create(PacketImpl * impl, iterator b, iterator e,
                                                         PacketInterpreterBase::Prepend_t)
            { return PacketInterpreter<T>::create(impl,b,e,PacketInterpreterBase::Prepend); }

        template <class T>
        static typename PacketInterpreter<T>::ptr create(PacketImpl * impl, iterator b, iterator e,
                                                         PacketInterpreterBase::ptr before)
            { return PacketInterpreter<T>::create(impl,b,e,before); }

        static PacketImpl * impl(PacketInterpreterBase::ptr p)
            { return &p->impl(); }
    };

}}}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "main.test.cci"
//#include "main.test.ct"
//#include "main.test.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
