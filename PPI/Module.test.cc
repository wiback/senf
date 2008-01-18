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
    \brief Module.test unit tests */

//#include "Module.test.hh"
//#include "Module.test.ih"

// Custom includes
#include "DebugEvent.hh"
#include "DebugModules.hh"
#include "Setup.hh"
#include "Module.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>
#include <boost/type_traits.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace ppi = senf::ppi;
namespace connector = ppi::connector;
namespace debug = ppi::module::debug;

namespace {
    class TestModule : public ppi::module::Module
    {
        SENF_PPI_MODULE(TestModule);

    public:
        connector::ActiveOutput<> output;

        ppi::DebugEvent event;

        TestModule() {
            noroute(output);
            registerEvent(event, &TestModule::onEvent);
        }

        void onEvent() {
            output(senf::DataPacket::create());
        }

        using ppi::module::Module::time;
    };
}

BOOST_AUTO_UNIT_TEST(module)
{
    // route and registerEvent are tested in Route.test.cc

    TestModule tester;
    debug::PassiveSink sink;
    ppi::connect(tester, sink);
    ppi::init();

    tester.event.trigger();
    BOOST_CHECK_EQUAL( sink.size(), 1u );
    BOOST_CHECK( senf::ClockService::now() - tester.time() < senf::ClockService::seconds(1) );
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
