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
    \brief ClockService public header */

#ifndef HH_SENF_Scheduler_ClockService_
#define HH_SENF_Scheduler_ClockService_ 1

// Custom includes
#include <boost/date_time/posix_time/posix_time.hpp>
#include <senf/config.hh>
#include <senf/Utils/singleton.hh>
#include <senf/Utils/Console/Parse.hh>

#ifdef SENF_DEBUG
#  include <senf/Utils/RestrictedInt.hh>
#endif

#ifdef SENF_DEBUG
#  define SENF_CLOCKSERVICE_CONSTEXPR
#else
#  ifdef SENF_CXX11_ENABLED
#    define SENF_CLOCKSERVICE_CONSTEXPR constexpr
#  else
#    define SENF_CLOCKSERVICE_CONSTEXPR
#  endif
#endif

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    // Implementation note: The clock value is represented as a 64bit unsigned integer number of
    // nanoseconds based on the CLOCK_MONOTONIC POSIX clock.
    //
    // To allow conversion between clock value and absolute time, the ClockService samples the
    // absolute current time and the clock value when the conversion is performed. This is done at
    // most once per second on a if-needed basis.

    /** \brief Reliable high precision monotonous clock source

        The ClockService provides a highly accurate monotonous clock source based on
        gettimeofday(). However, it takes additional precautions to detect clock skew.

        \implementation The funny mixture of static and non-static members stems from the old
            implementation based on interval timers and gettimeofday(). The current implementation
            uses POSIX clocks and is much simpler and more precise.
      */
//    namespace scheduler { ClockService::clock_type now(); }

    class ClockService
        : singleton<ClockService>
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        /** \brief ClockService timer data type

            Unsigned integer type representing scheduler time. Scheduler time is measured in
            nanoseconds relative to some implementation defined reference time.
         */
#ifdef SENF_DEBUG
        struct ClockTypeTag {};
        typedef senf::RestrictedInt<config::time_type, ClockTypeTag> clock_type;
#else
        typedef config::time_type clock_type;
#endif        
        /** \brief Supplementary integer type

            This type is used to represent varies supplementary values (e.g. number of microseconds)
         */
        typedef std::int64_t int64_type;

        /** \brief Absolute time data type

            <a href="http://www.boost.org/doc/libs/release/libs/date_time/index.html">Boost.DateTime</a>
            datatype used to represent absolute date/time values.
         */
        typedef boost::posix_time::ptime abstime_type;

        /** \brief Relative time data type

            <a href="http://www.boost.org/doc/libs/release/libs/date_time/index.html">Boost.DateTime</a>
            datatype used to represent time intervals
         */
        typedef boost::posix_time::time_duration reltime_type;

        //-////////////////////////////////////////////////////////////////////////

        static constexpr clock_type        maxTime = std::numeric_limits<config::time_type>::max();
        
        static clock_type now();  ///< Return current clock value

        static abstime_type abstime(clock_type const & clock); ///< Convert clock to absolute time
                                        /**< This member converts a clock value into an absolute
                                             <a href="http://www.boost.org/doc/libs/release/libs/date_time/index.html">Boost.DateTime</a>
                                             value.
                                             \note You should not base timeout calculations on this
                                                 absolute time value. Clock time is guaranteed to be
                                                 monotonous, absolute time may be non-monotonous if
                                                 the system date/time is changed. */

        static reltime_type reltime(clock_type const & clock); ///< Convert clock to relative time
                                        /**< This member converts a clock value into a relative
                                             <a href="http://www.boost.org/doc/libs/release/libs/date_time/index.html">Boost.DateTime</a>
                                             time interval
                                             \note The resolution of reltime_type might be smaller
                                                 than the clock_type resolution */

        static clock_type clock(abstime_type const & time); ///< Convert absolute time to clock value
                                        /**< This member convert an absolute time value into the
                                             corresponding clock value.
                                             \see abstime */

        static clock_type from_time_t(time_t const & time);
                                        ///< Convert legacy time_t to clock value
                                        /**< This member converts an absolute time value
                                             represented as a time_t value into a clock value */

        static time_t to_time_t(clock_type const & time);
                                        ///< Convert legacy time_t to clock value to legacy time_t
                                        /**< This member converts a clock value into an absolute time value
                                             represented as a time_t in UTC !*/

        static clock_type from_timeval(timeval const & time);
                                        ///< Convert legacy timeval to clock value
                                        /**< This member converts an absolute time value
                                             represented as a timeval value into a clock value */

        static SENF_CLOCKSERVICE_CONSTEXPR clock_type nanoseconds(int64_type const & v); ///< Convert \a v nanoseconds to clock_type
        static SENF_CLOCKSERVICE_CONSTEXPR clock_type microseconds(int64_type const & v); ///< Convert \a v microseconds to clock_type
        static SENF_CLOCKSERVICE_CONSTEXPR clock_type milliseconds(int64_type const & v); ///< Convert \a v milliseconds to clock_type
        static SENF_CLOCKSERVICE_CONSTEXPR clock_type seconds(int64_type const & v); ///< Convert \a v seconds to clock_type
        static SENF_CLOCKSERVICE_CONSTEXPR clock_type minutes(int64_type const & v); ///< Convert \a v minutes to clock_type
        static SENF_CLOCKSERVICE_CONSTEXPR clock_type hours(int64_type const & v); ///< Convert \a v hours to clock_type
        static SENF_CLOCKSERVICE_CONSTEXPR clock_type days(int64_type const & v); ///< Convert \a v days to clock_type

        static SENF_CLOCKSERVICE_CONSTEXPR int64_type in_nanoseconds(clock_type const & v); ///< Convert \a v to nanoseconds
        static SENF_CLOCKSERVICE_CONSTEXPR int64_type in_microseconds(clock_type const & v); ///< Convert \a v to microseconds
        static SENF_CLOCKSERVICE_CONSTEXPR int64_type in_milliseconds(clock_type const & v); ///< Convert \a v to milliseconds
        static SENF_CLOCKSERVICE_CONSTEXPR int64_type in_seconds(clock_type const & v); ///< Convert \a v to seconds
        static SENF_CLOCKSERVICE_CONSTEXPR int64_type in_minutes(clock_type const & v); ///< Convert \a v to minutes
        static SENF_CLOCKSERVICE_CONSTEXPR int64_type in_hours(clock_type const & v); ///< Convert \a v to hours
        static SENF_CLOCKSERVICE_CONSTEXPR int64_type in_days(clock_type const & v); ///< Convert \a v to days

        static void restart(); ///< Force re-synchronization of abstime and clock
                                        /**< Calling the member should never be necessary since
                                             abstime() / clock() automatically call restart() if
                                             needed */

    private:
        ClockService();

        abstime_type abstime_m(clock_type const & clock);
        clock_type clock_m(abstime_type const & time);
        void restart_m();

        boost::posix_time::ptime baseAbstime_;
        clock_type baseClock_;

        friend class singleton<ClockService>;
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

    void formatClockServiceInterval(ClockService::clock_type interval, std::ostream & os);
    
    class CyclicTimestamp final
    {
    public:
        CyclicTimestamp()
            : tstamp_(0) {
        }
        CyclicTimestamp(std::uint32_t const & secs, std::uint32_t const & nsecs) {
            update(secs, nsecs);
        }
        
        void update(ClockService::clock_type const & clockType) {
            tstamp_ = std::uint32_t(std::uint64_t(senf::ClockService::in_milliseconds(clockType)) % 0x100000000ull);
        }
        
        void update(std::uint32_t const & secs, std::uint32_t const & nsecs) {
            tstamp_ = (secs * 1000) + (nsecs / 1000000);
        }
        
        std::uint32_t const & value() const {
            return tstamp_;
        }
        
        CyclicTimestamp distance(CyclicTimestamp const & other) const {
            return CyclicTimestamp(other.tstamp_ - tstamp_);
        }

        std::uint32_t distanceAsMilliseconds(CyclicTimestamp const & other) const {
            return other.tstamp_ - tstamp_;
        }

        senf::ClockService::clock_type distanceAsClockType(CyclicTimestamp const & other) const {
            return senf::ClockService::milliseconds(distanceAsMilliseconds(other));
        }
        
    private:
        CyclicTimestamp(std::uint32_t && value)
            :tstamp_(value) {
        }

        std::uint32_t tstamp_;
    };
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
