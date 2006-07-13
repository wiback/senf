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

//#include "membind.test.hh"
//#include "membind.test.ih"

// Custom includes
#include <sstream>
#include <string>
#include "Utils/membind.hh"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    
    struct Test {
        char const * meth1() { 
            return "meth1()";
        }

        std::string meth2(int foo, int bar) {
            std::stringstream s;
            s << "meth2(" << foo << "," << bar << ")";
            return s.str();
        }
    };

}

BOOST_AUTO_UNIT_TEST(membind)
{
    Test instance;
    boost::function<char const * ()> f1 (satcom::lib::membind(&Test::meth1,instance));
    boost::function<std::string (int,int)> f2 (satcom::lib::membind(&Test::meth2,instance));

    BOOST_CHECK_EQUAL( f1(), "meth1()" );
    BOOST_CHECK_EQUAL( f2(1,2), "meth2(1,2)" );
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
