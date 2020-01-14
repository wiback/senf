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
    \brief Operators public header */

#ifndef HH_SENF_Utils_Operators_
#define HH_SENF_Utils_Operators_ 1

// Custom includes
#include <boost/operators.hpp>

//#include "Operators.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    template <class T, class B = boost::detail::empty_base<T> >
    struct strict_totally_ordered1
        : boost::totally_ordered1< T, boost::equivalent1< T, B > >
    {};

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "Operators.cci"
//#include "Operators.ct"
//#include "Operators.cti"
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
