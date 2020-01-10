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
    \brief membind unit tests */

#include "membind.hh"

// Custom includes
#include <sstream>
#include <string>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    struct Test {
        char const * meth1() {
            return "meth1()";
        }

        std::string meth2(int foo, int bar) const {
            std::stringstream s;
            s << "meth2(" << foo << "," << bar << ")";
            return s.str();
        }
    };

}

SENF_AUTO_TEST_CASE(membind)
{
    Test instance;
    boost::function<char const * ()> f1 (senf::membind(&Test::meth1,instance));
    boost::function<std::string (int,int)> f2 (senf::membind(&Test::meth2,instance));

    BOOST_CHECK_EQUAL( f1(), "meth1()" );
    BOOST_CHECK_EQUAL( f2(1,2), "meth2(1,2)" );
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
