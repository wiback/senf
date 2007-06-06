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
    \brief IteratorTraits public header */

#ifndef HH_IteratorTraits_
#define HH_IteratorTraits_ 1

// Custom includes
#include <vector>
#include <string>

//#include "IteratorTraits.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

// The following is *not* standard mandated but it *is* mandated by TR1 and I know of no
// implementation for which this is not correct

#define SENF_VECTOR_IS_CONTIGUOUS 1
#define SENF_STRING_IS_CONTIGUOUS 1
    
    /** \brief Check for contiguous mutable storage

        This type trait returns \c true, if \a RandomAccessIterator is an iterator into a contiguous
        storage area which may be written to. If this is the case, some algorithms may be optimized
        by directly modifying the underlying storage instead of relying on the STL interface.
        
        This trait is predefined to return \c true for pointers and for the iterators of \c
        std::vector and \c std::basic_string (and so for \c std::string and \c std::wstring). This
        is \e not required by the current standard. It is however required for \c std::vector in the
        first corrigendum to the standard, TR1. Furthermore almost all implementations for \c
        std::vector do follow this approach. 

        For \c std::string the case is different, there are libraries which use reference counting
        and shared ownership for strings, however no library with which SENF has been tested to date
        has strings of this variety. If SENF is used with such a standard library implementation,
        this header has to be adjysted to define the preprocessor symbol \c
        SENF_STRING_IS_CONTIGUOUS accordingly.
     */
    template <class RandomAccessIterator>
    struct contiguous_storage_iterator
        : public boost::false_type
    {};

    template <class T>
    struct contiguous_storage_iterator<T *>
        : public boost::true_type
    {};

#if defined(SENF_VECTOR_IS_CONTIGUOUS)
    template <class T, class Alloc>
    struct contiguous_storage_iterator< typename std::vector<T,Alloc>::iterator >
        : public boost::true_type
    {};
#endif

#if defined(SENF_STRING_IS_CONTIGUOUS)
    template <class CharT, class Traits, class Alloc>
    struct contiguous_storage_iterator< typename std::basic_string<CharT, Traits, Alloc>::iterator >
        : public boost::true_type
    {};
#endif

    /** \brief Convert contiguous storage iterator to pointer
        
        storage_iterator will convert a contiguous storage iterator into a pointer to the same
        element in the container. This allows to directly access the containers storage.
     */
    template <class Iterator>
    typename std::iterator_traits<Iterator>::pointer storage_iterator(Iterator i);
    
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "IteratorTraits.cci"
//#include "IteratorTraits.ct"
//#include "IteratorTraits.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
