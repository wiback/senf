// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

// Unit tests

//#include "TypeInfo.test.hh"
//#include "TypeInfo.test.ih"

// Custom includes
#include "TypeInfo.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace test {

    struct Base {
        virtual ~Base() {}
    };

    template <class C, int N>
    struct Foo : public Base
    {};

    enum Blub { A, B, C };
}

BOOST_AUTO_UNIT_TEST(prettyName)
{
    typedef test::Foo< test::Foo<test::Blub, 1>, 10> TestType;
    TestType ob;
    test::Base const & baseOb(ob);

    BOOST_CHECK_EQUAL( senf::prettyName(typeid(int)), "int");
    BOOST_CHECK_EQUAL( senf::prettyName(typeid(baseOb)), "test::Foo<test::Foo<test::Blub, 1>, 10>" );
}

///////////////////////////////cc.e////////////////////////////////////////
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
