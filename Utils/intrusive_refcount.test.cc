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

//#include "intrusive_refcount.test.hh"
//#include "intrusive_refcount.test.ih"

// Custom includes
#include "intrusive_refcount.hh"
#include <boost/intrusive_ptr.hpp>

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    struct Tester
        : public senf::intrusive_refcount
    {
        typedef boost::intrusive_ptr<Tester> ptr;

        Tester() { ++counter; }
        ~Tester() { --counter; }

        static unsigned counter;
    };

    unsigned Tester::counter = 0;
}

BOOST_AUTO_UNIT_TEST(intrusive_refcount)
{
    BOOST_CHECK_EQUAL(Tester::counter,0u);

    Tester::ptr p (new Tester);
    BOOST_CHECK_EQUAL(Tester::counter,1u);
    BOOST_CHECK_EQUAL(p->refcount(),1u);
    BOOST_CHECK_EQUAL(p->is_shared(),false);

    {
        Tester::ptr pp (p);
        BOOST_CHECK_EQUAL(Tester::counter,1u);
        BOOST_CHECK_EQUAL(p->refcount(),2u);
        BOOST_CHECK_EQUAL(p->is_shared(),true);
    }

    BOOST_CHECK_EQUAL(Tester::counter,1u);
    BOOST_CHECK_EQUAL(p->refcount(),1u);
    BOOST_CHECK_EQUAL(p->is_shared(),false);

    p = 0;
    BOOST_CHECK_EQUAL(Tester::counter,0u);
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
