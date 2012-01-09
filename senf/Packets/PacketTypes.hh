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
    \brief PacketTypes public header */

#ifndef HH_SENF_Packets_PacketTypes_
#define HH_SENF_Packets_PacketTypes_ 1

// Custom includes
#ifdef SENF_PACKET_STD_CONTAINER
#include <vector>
#else
#include "PacketVector.hh"
#endif
#include <boost/intrusive_ptr.hpp>
#include <boost/cstdint.hpp>
#include <senf/boost_intrusive/ilist.hpp>
#include <senf/boost_intrusive/ilist_hook.hpp>

//#include "PacketTypes.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    class PacketData;
    class PacketInterpreterBase;

namespace detail {

    class PacketImpl;

namespace packet {

    /** \brief Internal: Template typedef for used smart pointer

        \internal
     */
    template <class T>
    struct smart_pointer {
        typedef boost::intrusive_ptr<T> ptr_t;
    };

    struct interpreter_list_tag;
    typedef boost::intrusive::ilist_base_hook<interpreter_list_tag> interpreter_list_base;
    typedef interpreter_list_base::value_traits<PacketInterpreterBase> interpreter_list_type;

    typedef boost::intrusive::ilist<interpreter_list_type,false> interpreter_list;

    typedef boost::uint8_t byte;
#ifdef SENF_PACKET_STD_CONTAINER
    typedef std::vector<byte> raw_container;
#else
    typedef PacketVector raw_container;
#endif
    typedef raw_container::size_type size_type;
    typedef raw_container::difference_type difference_type;

    typedef raw_container::iterator iterator;
    typedef raw_container::const_iterator const_iterator;
    typedef long refcount_t; // This is long since boost uses long for refcounts .. hmm ..

namespace test {

    struct TestDriver;

}}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#endif
#if !defined(HH_SENF_Packets_Packets__decls_) && !defined(HH_SENF_Packets_PacketTypes_i_)
#define HH_SENF_Packets_PacketTypes_i_
//#include "PacketTypes.cci"
//#include "PacketTypes.ct"
//#include "PacketTypes.cti"
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
