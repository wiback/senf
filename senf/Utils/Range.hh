// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fit.fraunhofer.de/license.html
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
//   Stefan Bund <senf@g0dil.de>

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
