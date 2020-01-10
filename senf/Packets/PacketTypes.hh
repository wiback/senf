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
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/list_hook.hpp>

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
    typedef boost::intrusive::list_base_hook< boost::intrusive::tag<interpreter_list_tag> > interpreter_list_base;
    typedef boost::intrusive::list< PacketInterpreterBase,
                                    boost::intrusive::constant_time_size<false>,
                                    boost::intrusive::base_hook<interpreter_list_base> > interpreter_list;

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
