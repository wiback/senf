// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief PacketTypes public header */

#ifndef HH_PacketTypes_
#define HH_PacketTypes_ 1

// Custom includes
#include <vector>
#include <boost/intrusive_ptr.hpp>
#include <boost/cstdint.hpp>
#include <boost/intrusive/ilist.hpp>
#include <boost/intrusive/ilist_hook.hpp>

//#include "PacketTypes.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    class PacketData;
    class PacketInterpreterBase;

namespace detail {

    class PacketImpl;

namespace packet {

    template <class T>
    struct smart_pointer {
        typedef boost::intrusive_ptr<T> ptr_t;
    };
    
    struct interpreter_list_tag;
    typedef boost::intrusive::ilist_base_hook<interpreter_list_tag> interpreter_list_base;
    typedef interpreter_list_base::value_traits<PacketInterpreterBase> interpreter_list_type;

    typedef boost::intrusive::ilist<interpreter_list_type,false> interpreter_list;

    typedef boost::uint8_t byte;
    typedef std::vector<byte> raw_container;
    typedef raw_container::size_type size_type;
    typedef raw_container::difference_type difference_type;
    
    typedef raw_container::iterator iterator;
    typedef raw_container::const_iterator const_iterator;
    typedef long refcount_t; // This is long since boost uses long for refcounts .. hmm ..

namespace test {

    class TestDriver;

}}}}

///////////////////////////////hh.e////////////////////////////////////////
#endif
#if !defined(SENF_PACKETS_DECL_ONLY) && !defined(HH_PacketTypes_i_)
#define HH_PacketTypes_i_
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
