// $Id$
//
// Copyright (C) 2008 
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
    \brief AnnotationRouter.test unit tests */

//#include "AnnotationRouter.test.hh"
//#include "AnnotationRouter.test.ih"

// Custom includes
#include "AnnotationRouter.hh"
#include "DebugModules.hh"

#include "../Utils//auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

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
}

BOOST_AUTO_UNIT_TEST(annotationRouter)
{
    senf::ppi::module::debug::ActiveSource source;
    senf::ppi::module::debug::PassiveSink sink1;
    senf::ppi::module::debug::PassiveSink sink2;

    senf::ppi::module::AnnotationRouter<IntAnnotation> router;
    
    senf::ppi::connect(source, router);
    senf::ppi::connect(router, sink1, 1);
    senf::ppi::connect(router, sink2, 2);

    senf::ppi::init();

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
}

///////////////////////////////cc.e////////////////////////////////////////
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
