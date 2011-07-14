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
    \brief intrusive_refcount unit tests */

//#include "intrusive_refcount.test.hh"
//#include "intrusive_refcount.test.ih"

// Custom includes
#include "intrusive_refcount.hh"
#include <boost/intrusive_ptr.hpp>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

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

SENF_AUTO_UNIT_TEST(intrusive_refcount)
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

SENF_AUTO_UNIT_TEST(intrusive_refcount_t)
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
