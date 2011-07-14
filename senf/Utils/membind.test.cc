// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief membind unit tests */

//#include "membind.test.hh"
//#include "membind.test.ih"

// Custom includes
#include <sstream>
#include <string>
#include "membind.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

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

SENF_AUTO_UNIT_TEST(membind)
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
