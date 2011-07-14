// $Id$
//
// Copyright (C) 2009
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
    \brief Jack unit tests */

//#include "Jack.test.hh"
//#include "Jack.test.ih"

// Custom includes
#include "Jack.hh"
#include "DebugModules.hh"

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    class ActiveDummyForward
        : public senf::ppi::module::Module
    {
        SENF_PPI_MODULE(ActiveDummyForward);
    public:
        senf::ppi::connector::ActiveInput<> input;
        senf::ppi::connector::PassiveOutput<> output;

        ActiveDummyForward() : n (0)
            { route(input, output); output.onRequest(&ActiveDummyForward::request); }

        unsigned n;

    private:
        void request()
            { ++n; output(input()); }
    };

    class PassiveDummyForward
        : public senf::ppi::module::Module
    {
        SENF_PPI_MODULE(PassiveDummyForward);
    public:
        senf::ppi::connector::PassiveInput<> input;
        senf::ppi::connector::ActiveOutput<> output;

        PassiveDummyForward() : n (0)
            { route(input, output); input.onRequest(&PassiveDummyForward::request); }

        unsigned n;

    private:
        void request()
            { ++n; output(input()); }
    };

    struct ActiveGroup
    {
        senf::ppi::connector::ActiveInputJack<> input;
        senf::ppi::connector::PassiveOutputJack<> output;

        ActiveGroup()
            : input (forward1.input), output (forward1.output) {}

        void flip()
        {
            input.reset(forward2.input);
            output.reset(forward2.output);
        }

        ActiveDummyForward forward1;
        ActiveDummyForward forward2;
    };

    struct PassiveGroup
    {
        senf::ppi::connector::PassiveInputJack<> input;
        senf::ppi::connector::ActiveOutputJack<> output;

        PassiveGroup()
            : input (forward1.input), output (forward1.output) {}

        void flip()
        {
            input.reset(forward2.input);
            output.reset(forward2.output);
        }

        PassiveDummyForward forward1;
        PassiveDummyForward forward2;
    };

}

SENF_AUTO_UNIT_TEST(jacks)
{
    {
        ActiveGroup group;
        senf::ppi::module::debug::PassiveSource source;
        senf::ppi::module::debug::ActiveSink sink;

        senf::ppi::connect(source, group);
        senf::ppi::connect(group, sink);

        senf::ppi::init();

        {
            senf::Packet p (senf::DataPacket::create());
            source.submit(p);

            BOOST_CHECK(p == sink.request());
        }

        group.flip();
        senf::ppi::init();

        {
            senf::Packet p (senf::DataPacket::create());
            source.submit(p);

            BOOST_CHECK(p == sink.request());
        }

        BOOST_CHECK_EQUAL( group.forward1.n, 1u );
        BOOST_CHECK_EQUAL( group.forward2.n, 1u );
    }

    {
        PassiveGroup group;
        senf::ppi::module::debug::ActiveSource source;
        senf::ppi::module::debug::PassiveSink sink;

        senf::ppi::connect(source, group);
        senf::ppi::connect(group, sink);

        senf::ppi::init();

        {
            senf::Packet p (senf::DataPacket::create());
            source.submit(p);

            BOOST_CHECK(p == sink.pop_front());
        }

        group.flip();
        senf::ppi::init();

        {
            senf::Packet p (senf::DataPacket::create());
            source.submit(p);

            BOOST_CHECK(p == sink.pop_front());
        }

        BOOST_CHECK_EQUAL( group.forward1.n, 1u );
        BOOST_CHECK_EQUAL( group.forward2.n, 1u );
    }
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
