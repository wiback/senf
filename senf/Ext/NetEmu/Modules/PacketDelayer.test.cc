//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief PacketDelayer unit tests */

#include "PacketDelayer.hh"

// Unit test includes
#include <boost/test/floating_point_comparison.hpp>
#include <senf/Utils/auto_unit_test.hh>

// Custom includes
#include <senf/PPI/Setup.hh>
#include <senf/PPI/DiscardSink.hh>
#include <senf/Scheduler/Scheduler.hh>
#include <senf/Utils/membind.hh>
#include <senf/Ext/NetEmu/Annotations.hh>
#include <boost/random.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace ppi = senf::ppi;
namespace connector = ppi::connector;
namespace module = ppi::module;
namespace scheduler = senf::scheduler;

namespace {
    typedef senf::ClockService::clock_type clock_type;

     void useHiresTimer(bool hiresTimer_) {
        if (hiresTimer_) {
            if (scheduler::haveScalableHiresTimers())
                BOOST_MESSAGE( "using 'timerfd' hi-res timer.");
            else
                BOOST_MESSAGE( "using 'posix' hi-res timer.");
            scheduler::hiresTimers();
        }
        else
            BOOST_MESSAGE( "using 'epoll' lo-res timer.");
    }

    void timeout() {
        scheduler::terminate();
    }

    void run(clock_type t) {
        scheduler::TimerEvent timeoutTimer ("timeoutTimer", &timeout,
                senf::ClockService::now() + t);
        ppi::run();
    }

    class TestSource : public module::Module
    {
        SENF_PPI_MODULE( TestSource );
    public:
        connector::ActiveOutput<> output;

        TestSource() :
            random( 50, 150),
            timer_( "timer", senf::membind(&TestSource::timeout, this), randomTimeout())
        {
            noroute(output);
            generator.seed( senf::ClockService::in_nanoseconds( senf::ClockService::now()));
        }

    private:
        boost::uniform_smallint<> random;
        boost::mt19937 generator;
        scheduler::TimerEvent timer_;

        clock_type randomTimeout() {
            return scheduler::now() + senf::ClockService::milliseconds(random(generator));
        }

        void timeout() {
            senf::Packet p (senf::DataPacket::create());
            p.annotation<senf::emu::annotations::Timestamp>().value = senf::ClockService::now();
            output.write( p );
            timer_.timeout( randomTimeout());
        }
    };

    class TestSink : public module::Module
    {
        SENF_PPI_MODULE( TestSink );
    public:
        clock_type delay;
        connector::PassiveInput<> input;

        TestSink() : delay(0), tolerance(senf::ClockService::milliseconds(2)) {
            noroute(input);
            input.onRequest(&TestSink::onRequest);
        }
    private:
        clock_type tolerance;

        void check_not_to_early(clock_type a, clock_type b) {
            if ((a-b-delay) < senf::ClockService::clock_type(0))
                BOOST_CHECK_MESSAGE(false, "Packet arrived to early: "
                        << senf::ClockService::in_microseconds(a-b-delay) << " microseconds");
        }
        void check_not_to_late(clock_type a, clock_type b) {
            if ((a-b-delay) > tolerance)
                BOOST_WARN_MESSAGE(false, "Packet arrived to late: "
                    << senf::ClockService::in_microseconds(a-b-delay)
                    << " microseconds; tolerance=" << senf::ClockService::in_microseconds(tolerance)
                    << " microseconds");
        }

        void onRequest() {
            senf::Packet p = input.read();
            clock_type now (senf::ClockService::now());
            check_not_to_early( now, p.annotation<senf::emu::annotations::Timestamp>().value);
            check_not_to_late(  now, p.annotation<senf::emu::annotations::Timestamp>().value);
        }
    };
}

SENF_AUTO_TEST_CASE(packetDelayer)
{
    useHiresTimer( true);

    TestSource source;
    senf::emu::PacketDelayer delayer; // no delay
    ppi::connect( source, delayer);

    {
        // ok, first we must send a few packets to initialize internal ppi pools, queues etc,
        // these packets need more time
        module::DiscardSink discardSink;
        ppi::connect( delayer, discardSink);
        run( senf::ClockService::milliseconds(20));
        delayer.output.disconnect();
    }

    TestSink sink;
    ppi::connect( delayer, sink);

    BOOST_CHECK_EQUAL( delayer.delay(), senf::ClockService::clock_type(0));
    BOOST_MESSAGE("Entering sub test case \"no-delay\"");
    run( senf::ClockService::milliseconds(200));

    // now change delay to 200ms
    delayer.delay(senf::ClockService::milliseconds(200));
    BOOST_CHECK_EQUAL( delayer.delay(), senf::ClockService::milliseconds(200));
    sink.delay = delayer.delay();
    BOOST_MESSAGE("Entering sub test case \"with-delay 200ms\"");
    run( senf::ClockService::seconds(2));
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
