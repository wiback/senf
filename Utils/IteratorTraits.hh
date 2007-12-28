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
    \brief IteratorTraits public header */

#ifndef HH_IteratorTraits_
#define HH_IteratorTraits_ 1

// Custom includes
#include <boost/type_traits/integral_constant.hpp>
#include <vector>
#include <string>

//#include "IteratorTraits.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

// The following is *not* standard mandated but it *is* mandated by TR1 and I know of no
// implementation for which this is not correct

    /** \brief Check for contiguous mutable storage

        This type trait returns \c true, if \a RandomAccessIterator is an iterator into a contiguous
        storage area which may be written to. If this is the case, some algorithms may be optimized
        by directly modifying the underlying storage instead of relying on the STL interface.

        \code
        // Generic algorithm
        template <class Iterator>
        void do(Iterator i, boost::false_type)
        {
            // Access the iterator 'i' via the standard STL interface
        }

        template<class Iterator>
        void do(Iterator i, boost::true_type) 
        {
            typename Iterator::pointer p (senf::storage_iterator(i));
            // Manipulate the container by manipulating the data pointed at via 'p'
        }

        template <class Iterator>
        void foo(Iterator i)
        {
            // ...
            do( i, senf::contiguous_storage_iterator<Iterator>() );
            // ...
        }
        \endcode

        Thie \ref senf::storage_iterator helper function will convert an iterator to a pointer to
        the same element the iterator is referencing.
        
        This trait will return \c true for pointers. Additonally it should be configured to return
        true for all standard containers which obey above implementation restrictions. This
        typically includes \c std::vector and \c std::basic_string.

        To do so, the template must be specialized for those containers \c iterator type. If
        compiling with g++, this is implemented in \ref IteratorTraits.ih. This file should be
        extended for further compilers or STL implementations if needed.
     */
    template <class RandomAccessIterator>
    struct contiguous_storage_iterator
        : public boost::false_type
    {};

    /** \brief Check for contiguous mutable storage. Pointer specialization

        See \ref contiguous_storage_iterator.
     */
    template <class T>
    struct contiguous_storage_iterator<T *>
        : public boost::true_type
    {};

    /** \brief Convert contiguous storage iterator to pointer
        
        storage_iterator will convert a contiguous storage iterator into a pointer to the same
        element in the container. This allows to directly access the containers storage.

        \warning This conversion is only safe if \ref contiguous_storage_iterator<Iterator>::value
            is \c true for the given iterator type !
     */
    template <class Iterator>
    typename std::iterator_traits<Iterator>::pointer storage_iterator(Iterator i);
    
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "IteratorTraits.cci"
//#include "IteratorTraits.ct"
#include "IteratorTraits.cti"
#include "IteratorTraits.ih"
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
