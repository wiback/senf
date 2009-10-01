// $Id$
//
// Copyright (C) 2008 
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
    \brief Range public header */

#ifndef HH_SENF_Utils_Range_
#define HH_SENF_Utils_Range_ 1

// Custom includes
#include <boost/range/iterator_range.hpp>
#include <boost/iterator/transform_iterator.hpp>

//#include "Range.mpp"
///////////////////////////////hh.p////////////////////////////////////////

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

///////////////////////////////hh.e////////////////////////////////////////
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
