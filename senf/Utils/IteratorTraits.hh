//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief IteratorTraits public header */

#ifndef HH_SENF_Utils_IteratorTraits_
#define HH_SENF_Utils_IteratorTraits_ 1

// Custom includes
#include <boost/type_traits/integral_constant.hpp>
#include <vector>
#include <string>

//#include "IteratorTraits.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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

        This trait will return \c true for pointers. Additionally it should be configured to return
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
