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
    \brief AnnotationRouter unit tests */

#include "AnnotationRouter.hh"

// Custom includes
#include "DebugModules.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace ppi = senf::ppi;
namespace connector = ppi::connector;
namespace module = ppi::module;
namespace debug = module::debug;

namespace {
    struct IntAnnotation {
        int value;
        bool operator<(IntAnnotation const & other) const { return value < other.value; }
        bool operator==(IntAnnotation const & other) const { return value == other.value; }
        IntAnnotation() {}
        IntAnnotation(int v) : value(v) {}
        IntAnnotation & operator=(int v) { value=v; return *this; }
        operator int () const { return value; }
    };

    std::size_t hash_value(IntAnnotation const & i) { return i.value; }

    std::ostream & operator<<(std::ostream & os, IntAnnotation const & value)
    { os << value.value; return os; }

    struct AnnotationRouter : public module::AnnotationRouter<IntAnnotation>
    {
        using module::AnnotationRouter<IntAnnotation>::connectors;
    };
}

SENF_AUTO_TEST_CASE(annotationRouter)
{
    debug::ActiveSource source;
    debug::PassiveSink sink1;
    debug::PassiveSink sink2;

    AnnotationRouter router;

    ppi::connect(source, router);
    ppi::connect(router, 1, sink1);
    ppi::connect(router, 2, sink2);

    BOOST_CHECK_THROW( connect(router, 2, sink2),
            module::AnnotationRouter<IntAnnotation>::DuplicateKeyException);

    ppi::init();

    senf::Packet p1 (senf::DataPacket::create());
    p1.annotation<IntAnnotation>() = 1;
    senf::Packet p2 (senf::DataPacket::create());
    p2.annotation<IntAnnotation>() = 2;

    source.submit(p1);
    source.submit(p2);
    source.submit(senf::DataPacket::create());

    BOOST_CHECK_EQUAL( sink1.size(), 1u );
    BOOST_CHECK_EQUAL( sink2.size(), 1u );
    BOOST_CHECK( sink1.front() == p1 );
    BOOST_CHECK( sink2.front() == p2 );
    BOOST_CHECK_EQUAL(router.connectors().size(), 2u);

    sink1.input.disconnect();
    BOOST_CHECK_EQUAL(router.connectors().size(), 1u);

    senf::ppi::connect(router, 1, sink1);
    senf::ppi::init();
    source.submit(p1);

    BOOST_CHECK_EQUAL( sink1.size(), 2u );
    BOOST_CHECK_EQUAL( sink2.size(), 1u );
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
