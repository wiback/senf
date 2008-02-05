// $Id$
//
// Copyright (C) 2006
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

// Unit tests

//#include "intrusive_refcount.test.hh"
//#include "intrusive_refcount.test.ih"

// Custom includes
#include "intrusive_refcount.hh"
#include <boost/intrusive_ptr.hpp>

#include "../Utils/auto_unit_test.hh"
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

    struct TesterCustom
        : public senf::intrusive_refcount_t<TesterCustom>
    {
        typedef boost::intrusive_ptr<TesterCustom> ptr;
        typedef senf::intrusive_refcount_t<TesterCustom> super;

        TesterCustom() { ++counter; }
        ~TesterCustom() { --counter; }

        void add_ref() { super::add_ref(); ++refs; }
        bool release() { --refs; super::release(); return false; }

        static unsigned counter;
        static unsigned refs;
    };

    unsigned TesterCustom::counter = 0;
    unsigned TesterCustom::refs = 0;
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

BOOST_AUTO_UNIT_TEST(intrusive_refcount_t)
{
    BOOST_CHECK_EQUAL(TesterCustom::counter,0u);
    BOOST_CHECK_EQUAL(TesterCustom::refs,0u);

    TesterCustom::ptr p (new TesterCustom);
    BOOST_CHECK_EQUAL(TesterCustom::counter,1u);
    BOOST_CHECK_EQUAL(p->refcount(),1u);
    BOOST_CHECK_EQUAL(p->is_shared(),false);
    BOOST_CHECK_EQUAL(TesterCustom::refs,1u);
    

    {
        TesterCustom::ptr pp (p);
        BOOST_CHECK_EQUAL(TesterCustom::counter,1u);
        BOOST_CHECK_EQUAL(p->refcount(),2u);
        BOOST_CHECK_EQUAL(p->is_shared(),true);
        BOOST_CHECK_EQUAL(TesterCustom::refs,2u);
    }

    BOOST_CHECK_EQUAL(TesterCustom::counter,1u);
    BOOST_CHECK_EQUAL(p->refcount(),1u);
    BOOST_CHECK_EQUAL(p->is_shared(),false);
    BOOST_CHECK_EQUAL(TesterCustom::refs,1u);

    {
        TesterCustom * pp (p.get());
        p = 0;
        BOOST_CHECK_EQUAL(TesterCustom::counter,1u);
        BOOST_CHECK_EQUAL(TesterCustom::refs,0u);
        // The TesterCustom leaks ...
        delete pp;
        BOOST_CHECK_EQUAL(TesterCustom::counter,0u);
    }
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
