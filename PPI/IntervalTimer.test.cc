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
    \brief IntervalTimer.test unit tests */

//#include "IntervalTimer.test.hh"
//#include "IntervalTimer.test.ih"

// Custom includes
#include "IntervalTimer.hh"
#include "Module.hh"
#include "Setup.hh"

#include "../Utils/auto_unit_test.hh"
#include <boost/test/test_tools.hpp>

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace module = senf::ppi::module;
namespace ppi = senf::ppi;

namespace {
    
    class TimerTest : public module::Module
    {
        SENF_PPI_MODULE(TimerTest);

        ppi::IntervalTimer timer;

        void tick(ppi::IntervalTimer::Event const & event) {
            if ( --n == 0 )
                timer.enabled(false);
        }

        unsigned n;

    public:
        TimerTest(senf::ClockService::clock_type d_, unsigned n_)
        :   timer ( senf::ClockService::milliseconds(d_) ), 
            n     ( n_ ) 
        {
            registerEvent( timer, &TimerTest::tick );
        }
    };

    bool is_close_clock(senf::ClockService::clock_type a, senf::ClockService::clock_type b, 
                        unsigned long delta = 50000000ul)
    {
        return (a<b ? b-a : a-b ) < delta;
    }
}

BOOST_AUTO_UNIT_TEST(intervalTimer)
{
    TimerTest timer (100,4);
    senf::ClockService::clock_type start (senf::ClockService::now());
    senf::ppi::run();
    BOOST_CHECK_PREDICATE( is_close_clock, 
                           (senf::ClockService::now())
                           (start+senf::ClockService::milliseconds(400)) );
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
