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
    \brief intrusive_refcount unit tests */

#include "intrusive_refcount.hh"

// Custom includes
#include <boost/intrusive_ptr.hpp>

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

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

SENF_AUTO_TEST_CASE(intrusive_refcount)
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

SENF_AUTO_TEST_CASE(intrusive_refcount_t)
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
