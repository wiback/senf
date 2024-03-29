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
    \brief IteratorTraits unit tests */

#include "IteratorTraits.hh"

// Custom includes
#include <vector>
#include <string>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

SENF_AUTO_TEST_CASE(iteratorTraits)
{
    BOOST_CHECK_EQUAL( senf::contiguous_storage_iterator<int*>::value, true );
    BOOST_CHECK_EQUAL( senf::contiguous_storage_iterator<void>::value, false );
#if defined(__GNUG__) && ! defined(_GLIBCXX_DEBUG)
    BOOST_CHECK_EQUAL( senf::contiguous_storage_iterator<std::vector<int>::iterator>::value, true);
    BOOST_CHECK_EQUAL( senf::contiguous_storage_iterator<std::string::iterator>::value, true);
#endif
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
