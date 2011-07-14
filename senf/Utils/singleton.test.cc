// $Id$
//
// Copyright (C) 2007
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
    \brief singleton unit tests */

//#include "singleton.test.hh"
//#include "singleton.test.ih"

// Custom includes
#include <iostream>
#include "singleton.hh"
#include "IgnoreValue.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    class Test : public senf::singleton<Test>
    {
        friend class senf::singleton<Test>;

        Test() : foo_(1234) {}

        int foo_;

    public:
        using senf::singleton<Test>::instance;
        using senf::singleton<Test>::alive;

        int foo() { return foo_; }
    };
}

SENF_AUTO_UNIT_TEST(sInGlEtOn)
{
    BOOST_CHECK_EQUAL( Test::instance().foo(), 1234 );
    BOOST_CHECK( Test::alive() );
}

namespace {

    bool test1Dead (false);
    bool test2Dead (false);

    bool test1Alive (false);
    bool test2Alive (false);

    struct AliveTest1 : public senf::singleton<AliveTest1>
    {
        friend class senf::singleton<AliveTest1>;
        using senf::singleton<AliveTest1>::alive;
        using senf::singleton<AliveTest1>::instance;
        AliveTest1();
        ~AliveTest1();
    };

    struct AliveTest2 : public senf::singleton<AliveTest2>
    {
        friend class senf::singleton<AliveTest2>;
        using senf::singleton<AliveTest2>::alive;
        using senf::singleton<AliveTest2>::instance;
        AliveTest2();
        ~AliveTest2();
    };

    AliveTest1::AliveTest1()
    {
        test2Alive = AliveTest2::alive();
    }

    AliveTest1::~AliveTest1()
    {
        if (test2Dead) {
            if (! AliveTest2::alive() )
                std::cerr << "singleton alive test ok\n";
            else {
                std::cerr << "singleton alive test 2 NOT ok!\n";
                throw 1;
            }
        }
        test1Dead = true;
    }

    AliveTest2::AliveTest2()
    {
        test1Alive = AliveTest1::alive();
    }

    AliveTest2::~AliveTest2()
    {
        if (test1Dead) {
            if (! AliveTest1::alive() )
                std::cerr << "singleton alive test ok\n";
            else {
                std::cerr << "singleton alive test 1 NOT ok!\n";
                throw 1;
            }
        }
        test2Dead = true;
    }

}

SENF_AUTO_UNIT_TEST(singletonAlive)
{
    senf::IGNORE( AliveTest1::instance() );
    senf::IGNORE( AliveTest2::instance() );

    BOOST_CHECK( (test1Alive && !test2Alive) || (!test1Alive && test2Alive) );
    BOOST_CHECK( AliveTest1::alive() );
    BOOST_CHECK( AliveTest2::alive() );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
