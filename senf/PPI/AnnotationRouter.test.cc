// $Id$
//
// Copyright (C) 2008
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
    \brief AnnotationRouter unit tests */

//#include "AnnotationRouter.test.hh"
//#include "AnnotationRouter.test.ih"

// Custom includes
#include "AnnotationRouter.hh"
#include "DebugModules.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

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

    std::ostream & operator<<(std::ostream & os, IntAnnotation const & value)
    { os << value.value; return os; }

    struct AnnotationRouter : public module::AnnotationRouter<IntAnnotation>
    {
        using module::AnnotationRouter<IntAnnotation>::connectors;
    };
}

SENF_AUTO_UNIT_TEST(annotationRouter)
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
