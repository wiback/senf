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
    \brief TimerEvent unit tests */

//#include "TimerEvent.test.hh"
//#include "TimerEvent.test.ih"

// Custom includes
#include "TimerEvent.hh"
#include "Scheduler.hh"
#include <boost/bind.hpp>

#include <senf/Utils/auto_unit_test.hh>
#include <boost/test/test_tools.hpp>
#include <boost/random.hpp>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    bool is_close(senf::ClockService::clock_type a, senf::ClockService::clock_type b)
    {
        return (a<b ? b-a : a-b) < senf::ClockService::milliseconds(50);
    }

    bool called = false;
    void handler()
    {
        called = true;
    }

}

SENF_AUTO_UNIT_TEST(timerDispatcher)
{
    char const * enabled (getenv("SENF_TIMING_CRITICAL_TESTS"));
    BOOST_WARN_MESSAGE(enabled, "Set SENF_TIMING_CRITICAL_TESTS to not skip timing critical tests");

    senf::scheduler::detail::FdManager::instance().timeout(1000);

    senf::ClockService::clock_type t (senf::ClockService::now());
    {
        senf::scheduler::TimerEvent timer ("testTimer", &handler,
                                           t + senf::ClockService::milliseconds(500));
        SENF_CHECK_NO_THROW( timer.disable() );
        SENF_CHECK_NO_THROW( timer.enable() );
        BOOST_CHECK( timer.enabled() );
        SENF_CHECK_NO_THROW( senf::scheduler::process() );
        senf::ClockService::clock_type t2 (senf::ClockService::now());
        BOOST_CHECK( called );
        BOOST_CHECK( ! timer.enabled() );
        if (enabled)
            BOOST_CHECK_PREDICATE( is_close, (t2-t)(senf::ClockService::milliseconds(500)) );

        called=false;
        t = senf::ClockService::now();
        SENF_CHECK_NO_THROW( timer.timeout(t) );
        BOOST_CHECK( timer.enabled() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::TimerDispatcher::instance().enable() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::TimerDispatcher::instance().reschedule() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::FdManager::instance().processOnce() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::TimerDispatcher::instance().prepareRun() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::FIFORunner::instance().run() );
        SENF_CHECK_NO_THROW( senf::scheduler::detail::TimerDispatcher::instance().disable() );
        if (enabled)
            BOOST_CHECK_PREDICATE( is_close, (t) (senf::ClockService::now()) );
        BOOST_CHECK( called );
    }
}

namespace {

    senf::ClockService::clock_type randomDelay()
    {
        static boost::uniform_smallint<> random (100,300);
        static boost::mt19937 generator;
        return senf::scheduler::now() + senf::ClockService::milliseconds(random(generator));
    }

    unsigned count (0);
    senf::ClockService::clock_type delay (0);
    bool haveCb (false);

    void jitterCb(senf::scheduler::TimerEvent & tm)
    {
        //std::cerr << "diff:" << senf::ClockService::in_microseconds( senf::scheduler::now() - tm.timeout()) << '\n';
        count ++;
        delay += senf::ClockService::in_microseconds( senf::scheduler::now() - tm.timeout());
        haveCb = true;
        tm.timeout(randomDelay());
    }

    void preCb()
    {
        haveCb = false;
    }

    void postCb()
    {
        if (! haveCb)
            std::cerr << senf::scheduler::now() << '\n';
    }

    void idleCb(senf::scheduler::TimerEvent & tm)
    {
        tm.timeout( senf::scheduler::now());
    }

    void jitterTest()
    {
        count = 0;
        delay = 0;
//        senf::scheduler::EventHook pre ("jitterTest::preCb", &preCb,
//                                        senf::scheduler::EventHook::PRE);
//        senf::scheduler::EventHook post ("jitterTest::postCb", &postCb,
//                                         senf::scheduler::EventHook::POST);

        senf::scheduler::TimerEvent tm1 (
                "jitterTest::tm1", boost::bind(&jitterCb, boost::ref(tm1)), randomDelay());
        senf::scheduler::TimerEvent tm2 (
                "jitterTest::tm2", boost::bind(&jitterCb, boost::ref(tm2)), randomDelay());
        senf::scheduler::TimerEvent tm3 (
                "jitterTest::tm3", boost::bind(&jitterCb, boost::ref(tm3)), randomDelay());

        // enabled "Idle"-Event will degrade scheduling delay
        //senf::scheduler::TimerEvent idle (
        //        "jitterTest::idle", boost::bind(&idleCb, boost::ref(idle)), senf::scheduler::now());

        senf::scheduler::TimerEvent timeout("jitterTest::timeout", &senf::scheduler::terminate,
                                            senf::scheduler::now() + senf::ClockService::seconds(5));

        senf::scheduler::process();

        std::cerr << "Average scheduling delay: " << delay/count << " microseconds\n";
    }

}

SENF_AUTO_UNIT_TEST(timerJitter)
{
    senf::scheduler::watchdogTimeout(0);
    std::cerr << "Epoll timers\n";
    senf::scheduler::loresTimers();
    jitterTest();
    std::cerr << "Hires timers\n";
    senf::scheduler::hiresTimers();
    jitterTest();
    senf::scheduler::loresTimers();
    senf::scheduler::watchdogTimeout(1000);
    BOOST_CHECK( true );
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
