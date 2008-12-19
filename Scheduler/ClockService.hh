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
    \brief ClockService public header */

#ifndef HH_SENF_Scheduler_ClockService_
#define HH_SENF_Scheduler_ClockService_ 1

// Custom includes
#include <sys/time.h> 
#include <boost/utility.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/cstdint.hpp>
#include "../config.hh"
#include "../Utils/singleton.hh"
#include "../Utils/Console/Parse.hh"

//#include "ClockService.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

#ifndef DOXYGEN
    namespace detail { class ClockServiceTest; }
#endif

    // Implementation note: The clock value is represented as a 64bit unsigned integer number of
    // nanosecods based on the CLOCK_MONOTONIC POSIX clock.
    //
    // To allow conversion between clock value and absolute time, the ClockService samples the
    // absolute current time and the clock value when the conversion is performed. This is done at
    // most once per second on a if-needed basis.
    
    /** \brief Reliable high precision monotonous clock source

        The ClockService provides a highly accurate monotonous clock source based on
        gettimeofday(). However, it takes additional precautions to detect clock skew.

        \implementation The funny mixture of static and non-static members stems from the old
            implementation based on interval timers and gettimeofday(). The current implementation
            usses POSIX clocks and is much simpler and more precise.
      */
    class ClockService
        : singleton<ClockService>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        /** \brief ClockService timer data type
            
            Unsigned integer type representing scheduler time. Scheduler time is measured in
            nanoseconds relative to some implementation defined reference time.
         */
        typedef config::time_type clock_type;

        /** \brief Supplementary integer type

            This type is used to represent varies supplementary values (e.g. number of microseconds)
         */
        typedef boost::int_fast64_t int64_type;

        /** \brief Absolute time data type

            Boost.DateTime datatype used to represent absolute date/time values.
         */
        typedef boost::posix_time::ptime abstime_type;

        /** \brief Relative time data type

            Boost.DateTime datatype used to represent time intervals
         */
        typedef boost::posix_time::time_duration reltime_type;

        ///////////////////////////////////////////////////////////////////////////

        static clock_type now();  ///< Return current clock value
        
        static abstime_type abstime(clock_type clock); ///< Convert clock to absolute time
                                        /**< This member converts a clock value into an absolute
                                             Boost.DateTime value.
                                             \note You should not base timeout calculations on this
                                                 absolute time value. Clock time is guaranteed to be
                                                 monotonous, absolute time may be non-monotonous if
                                                 the system date/time is changed. */

        static reltime_type reltime(clock_type clock); ///< Convert clock to relative time
                                        /**< This member converts a clock value into a relative
                                             Boost.DateTime time interval
                                             \note The resolution of reltime_type might be smaller
                                                 than the clock_type resolution */

        static clock_type clock(abstime_type time); ///< Convert absolute time to clock value
                                        /**< This member converst an absolute time value into the
                                             corresponding clock value.
                                             \see abstime */

        static clock_type from_time_t(time_t const & time); 
                                        ///< Convert legacy time_t to clock value
                                        /**< This member converts an absolute time value 
                                             represented as a time_t value into a clock value */

        static clock_type from_timeval(timeval const & time); 
                                        ///< Convert legacy timeval to clock value
                                        /**< This member converts an absolute time value
                                             represented as a timeval value into a clock value */

        static clock_type nanoseconds(int64_type v); ///< Convert \a v nanoseconds to clock_type
        static clock_type microseconds(int64_type v); ///< Convert \a v microseconds to clock_type
        static clock_type milliseconds(int64_type v); ///< Convert \a v milliseconds to clock_type
        static clock_type seconds(int64_type v); ///< Convert \a v seconds to clock_type
        static clock_type minutes(int64_type v); ///< Convert \a v minutes to clock_type
        static clock_type hours(int64_type v); ///< Convert \a v hours to clock_type
        static clock_type days(int64_type v); ///< Convert \a v days to clock_type

        static int64_type in_nanoseconds(clock_type v); ///< Convert \a v to nanoseconds
        static int64_type in_microseconds(clock_type v); ///< Convert \a v to microseconds
        static int64_type in_milliseconds(clock_type v); ///< Convert \a v to milliseconds
        static int64_type in_seconds(clock_type v); ///< Convert \a v to seconds
        static int64_type in_minutes(clock_type v); ///< Convert \a v to minutes
        static int64_type in_hours(clock_type v); ///< Convert \a v to hours
        static int64_type in_days(clock_type v); ///< Convert \a v to days

        static void restart(); ///< Force re-syncronisation of abstime and clock
                                        /**< Calling the member should never be necessary since
                                             abstime() / clock() automatically call restart() if
                                             needed */

    private:
        ClockService();

        abstime_type abstime_m(clock_type clock);
        clock_type clock_m(abstime_type time);
        void restart_m();

        boost::posix_time::ptime baseAbstime_;
        clock_type baseClock_;

        /// Internal: ClockService private data (PIMPL idiom)

#ifndef DOXYGEN
        friend class singleton<ClockService>;
#endif
    };

    /** \brief Console argument parser to parse value as time interval

        This parser will parse a time interval specification into a ClockService::clock_type
        value. The following units are supported:

        <table class="senf fixedcolumn">
        <tr><td>\c d</td><td>days</td></tr>
        <tr><td>\c h</td><td>hours</td></tr>
        <tr><td>\c m</td><td>minutes</td></tr>
        <tr><td>\c s</td><td>seconds</td></tr>
        </table>

        Additionally, the unit may be prefixed by an SI scale:

        <table class="senf fixedcolumn">
        <tr><td>\c m</td><td>milli</td></tr>
        <tr><td>\c u</td><td>micro</td></tr>
        <tr><td>\c n</td><td>nano</td></tr>
        </table>

        An optional decimal point is also supported. A single timer interval may combine any number
        of these specifications. The following are all valid intervals:

        <table class="senf fixedcolumn">
        <tr><td><code>10d</code></td><td>10 days</td></tr>
        <tr><td><code>5d5d</code></td><td>10 days</td></tr>
        <tr><td><code>1d2h100m3.5s</code></td><td>27 hours, 30 minutes and 3.5 seconds</td></tr>
        <tr><td><code>1s100ms</code></td><td>1.1 seconds</td></tr>
        <tr><td><code>1.1s</code></td><td>1.1 seconds</td></tr>
        <tr><td><code>123.456us</code></td><td>123.456 microseconds</td></tr>
        <tr><td><code>2md</code></td><td>(very unusual) 2 milli-days</td></tr>
        </table>
     */
    void parseClockServiceInterval(console::ParseCommandInfo::TokensRange const & tokens, 
                                   ClockService::clock_type & out);
}

///////////////////////////////hh.e////////////////////////////////////////
#include "ClockService.cci"
//#include "ClockService.ct"
//#include "ClockService.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
