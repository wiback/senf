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
    \brief main.test public header */

#ifndef HH_SENF_Packets_main_test_
#define HH_SENF_PAckets_main_test_ 1

// Custom includes

//#include "main.test.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
