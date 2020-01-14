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
    \brief Range public header */

#ifndef HH_SENF_Utils_Range_
#define HH_SENF_Utils_Range_ 1

// Custom includes
#include <boost/range/iterator_range.hpp>
#include <boost/iterator/transform_iterator.hpp>

//#include "Range.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Make a \c boost::iterator_range from \c boost::trasform_iterator

        This helper is like \c boost::make_transform_iterator, however for ranges instead of
        iterators.
     */
    template <class Range, class Fn>
    boost::iterator_range<
        boost::transform_iterator< Fn,
                                   typename boost::range_const_iterator<Range>::type > >
    make_transform_range(Range const & range, Fn const & fn);

    template <class Range, class Fn>
    boost::iterator_range<
        boost::transform_iterator< Fn,
                                   typename boost::range_iterator<Range>::type > >
    make_transform_range(Range & range, Fn const & fn);

    template <class Range, class Iterator>
    void copy_range(Range const & range, Iterator const & i);

    template <class Range, class Iterator>
    void copy_range(Range & range, Iterator const & i);

    template <class Range>
    bool has_one_elt(Range r);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "Range.cci"
//#include "Range.ct"
#include "Range.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
