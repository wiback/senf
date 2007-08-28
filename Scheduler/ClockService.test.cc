// $Id$
//
// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief ClockService.test unit tests */

//#include "ClockService.test.hh"
//#include "ClockService.test.ih"

// Custom includes
#include "ClockService.hh"
#include <errno.h>

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace senf {
namespace detail {

    struct ClockServiceTest
    {
        static boost::posix_time::ptime & base() 
            { return senf::ClockService::instance().base_; }
        static boost::posix_time::ptime & heartbeat()
            { return senf::ClockService::instance().heartbeat_; }
    };

}}

namespace {

    bool is_close_clock(senf::ClockService::clock_type a, senf::ClockService::clock_type b, 
                        unsigned long delta = 50000000ul)
    {
        return (a<b ? b-a : a-b ) < delta;
    }

    bool is_close_pt(boost::posix_time::ptime a, boost::posix_time::ptime b,
                     boost::posix_time::time_duration delta = boost::posix_time::milliseconds(50) )
    {
        return (a<b ? b-a : a-b ) < delta;
    }

    void delay(unsigned long milliseconds)
    {
        struct timespec ts;
        ts.tv_sec = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;
        while (nanosleep(&ts,&ts) < 0 && errno == EINTR) ;
    }

}

BOOST_AUTO_UNIT_TEST(clockService)
{
    senf::ClockService::restart(); // So we know, when the signal will be delivered
    
    senf::ClockService::clock_type t1 (senf::ClockService::now());
    delay(200);
    senf::ClockService::clock_type t2 (senf::ClockService::now());
    BOOST_CHECK_PREDICATE( is_close_clock,
                           (t1 + senf::ClockService::milliseconds(200)) 
                           (t2) );

    t1 = t2;
    // We shift both heartbeat() and base() back 1 minute. This is the same as
    // moving the current time forward 1 minute.
    boost::posix_time::ptime b (senf::detail::ClockServiceTest::base());
    boost::posix_time::ptime h (senf::detail::ClockServiceTest::heartbeat());
    senf::detail::ClockServiceTest::heartbeat() -= boost::posix_time::minutes(1);
    senf::detail::ClockServiceTest::base() -= boost::posix_time::minutes(1);

    // Wait for SIGALRM and let the signal handler do the clock-skew detection
    delay(senf::ClockService::CheckInterval*1000);

    BOOST_CHECK_PREDICATE( is_close_pt,
                           (b)
                           (senf::detail::ClockServiceTest::base()) );
    BOOST_CHECK_PREDICATE( is_close_pt,
                           (h+boost::posix_time::seconds(senf::ClockService::CheckInterval))
                           (senf::detail::ClockServiceTest::heartbeat()) );

    t2 = senf::ClockService::now();
    BOOST_CHECK_PREDICATE( is_close_clock,
                           (t1 + senf::ClockService::seconds(senf::ClockService::CheckInterval))
                           (t2) );

    t1 = t2;

    senf::detail::ClockServiceTest::heartbeat() -= boost::posix_time::minutes(1);
    senf::detail::ClockServiceTest::base() -= boost::posix_time::minutes(1);

    // Let now() do the clock skew detection using getitimer() ...
    delay(200);
    BOOST_CHECK_PREDICATE( is_close_clock,
                           (t1 + senf::ClockService::milliseconds(200))
                           (senf::ClockService::now()) );
    
    // We need restart the clock-service here to fix up the bad heartbeat value. That's faster
    // than waiting another CheckInterval seconds for SIGALRM
    senf::ClockService::restart();
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
