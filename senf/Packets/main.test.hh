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
    \brief main.test public header */

#ifndef HH_SENF_Packets_main_test_
#define HH_SENF_Packets_main_test_ 1

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
