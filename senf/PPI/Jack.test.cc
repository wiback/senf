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
    \brief Jack unit tests */

#include "Jack.hh"

// Custom includes
#include "DebugModules.hh"

// Unit test includes
#include <senf/Utils/auto_unit_test.hh>

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

SENF_AUTO_TEST_CASE(jacks)
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
