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
