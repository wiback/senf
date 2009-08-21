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
    \brief Queueing unit tests */

//#include "Queueing.test.hh"
//#include "Queueing.test.ih"

// Custom includes
#include "Queueing.hh"
#include "Module.hh"
#include "Connectors.hh"
#include "DebugModules.hh"
#include <senf/Packets/Packets.hh>
#include "Setup.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace ppi = senf::ppi;
namespace connector = ppi::connector;
namespace module = ppi::module;
namespace debug = module::debug;

namespace {
    class QueueTester : public module::Module
    {
        SENF_PPI_MODULE(QueueTester);
    public:
        connector::PassiveInput<> input;
        connector::ActiveOutput<> output;

        QueueTester() {
            route(input, output);
            input.qdisc(ppi::ThresholdQueueing(2,1));
            input.onRequest(&QueueTester::nop);
        }

        void nop() { /* no operation */ }

        void forward() {
            if (input && output)
                output(input());
        }

    };
}

BOOST_AUTO_UNIT_TEST(thresholdQueueing)
{
    debug::ActiveSource source;
    QueueTester tester;
    debug::PassiveSink sink;

    ppi::connect(source, tester);
    ppi::connect(tester, sink);
    ppi::init();

    senf::Packet p (senf::DataPacket::create());
    BOOST_CHECK( source );
    source.submit(p);
    BOOST_CHECK( source );
    source.submit(p);
    BOOST_CHECK( ! source );
    BOOST_CHECK_EQUAL( tester.input.queueSize(), 2u );
    tester.forward();
    BOOST_CHECK_EQUAL( tester.input.queueSize(), 1u );
    BOOST_CHECK( source );
    tester.forward();
    BOOST_CHECK_EQUAL( tester.input.queueSize(), 0u );
    BOOST_CHECK( source );
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
