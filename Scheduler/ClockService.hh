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

#ifndef HH_ClockService_
#define HH_ClockService_ 1

// Custom includes
#include <sys/time.h> 
#include <boost/utility.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/scoped_ptr.hpp>
#include "../Utils/singleton.hh"

//#include "ClockService.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

#ifndef DOXYGEN
    namespace detail { class ClockServiceTest; }
#endif

    // Implementation note:
    //
    // The clock value is represented as a 64bit unsigned integer number of nanosecods elapsed since
    // the construction of the ClockService object. 
    // 
    // The implementation must provide two features:
    // a) It must reliably detect clock changes
    // b) In case of a clock change a reasonably accurate fallback clock value must be provided
    //
    // We do this using setitimer/getitimer. We setup an interval timer sending SIGALRM whenever
    // CheckInverval seconds have elapsed.
    //
    // On every SIGALRM signal we save the current value of gettimeofday(). If this new value is
    // substantially different from the currently saved value + CheckInterval, the clock has been
    // changed.
    //
    // Whenever the current clock value is requested using now(), the current gettimeofday() value
    // is compared with the saved value. If the difference is substantially more than CheckInterval,
    // the clock has been changed.
    //
    // This provides clock skew detection. If clock skew is detected, we need to move base_ by the
    // amount the time has been changed. To do this we need an as accurate as possible approximation
    // of the expected current time value. We need to differentiate two cases:
    //
    // a) Clock skew detected within now()
    //
    // In this case, we use getitimer() to find the time remaining in the timer. Using this value
    // and the saved gettimeofday() value we can adjust base_ accordingly.
    //
    // b) Clock skew detected in the signal handler
    //
    // In this case we use the saved gettimeofday() value + CheckInterval to adjust base_.
    
    /** \brief Reliable high precision monotonous clock source

        The ClockService provides a highly accurate monotonous clock source based on
        gettimeofday(). However, it takes additional precautions to detect clock skew.

        \implementation We use a mix of static and non-static members to achieve high performance
            in the normal case (no clock skew) and still encapsulate the dependency on legacy C
            headers. Using the senf::singleton mixin ensures, that the instance is constructed
            before main even when instance() is not called.

        \bug There is a deadlock condition between ClockService and the streaming of Boost.DateTime
            values: Boost.DateTime seems to call tzset() whenever writing a date/time value (ugh)
            and since tzset changes basic date/time values, it seems to block gettimeofday() which
            leads to the SIGLARM handler blocking indefinitely. Resolution either a) find out, why
            tzset() of all functions is called or b) move the ClockService heartbeat functionality
            into the Scheduler.
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
        typedef boost::int_fast64_t clock_type;
        typedef boost::int_fast64_t int64_type;

        /** \brief Absolute time data type

            Boost.DateTime datatype used to represent absolute date/time values.
         */
        typedef boost::posix_time::ptime abstime_type;

        static unsigned const CheckInterval = 10;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        ~ClockService();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        static clock_type now();  ///< Return current clock value
        
        static abstime_type abstime(clock_type clock); ///< Convert clock to absolute time
                                        /**< This member converts a clock value into an absolute
                                             Boost.DateTime value.
                                             \note You should not base timeout calculations on this
                                                 absolute time value. Clock time is guaranteed to be
                                                 monotonous, absolute time may be non-monotonous if
                                                 the system date/time is changed. */

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

        static void restart();

    private:
        ClockService();

        void timer();

        clock_type now_m();
        abstime_type abstime_m(clock_type clock);
        clock_type clock_m(abstime_type time);
        void restart_m(bool restart = true);

        bool checkSkew(boost::posix_time::ptime time);
        void updateSkew(boost::posix_time::ptime time);
        void clockSkew(boost::posix_time::ptime time, boost::posix_time::ptime expected);

        void restartTimer(bool restart = true);

        boost::posix_time::ptime base_;
        boost::posix_time::ptime heartbeat_;

        // I don't want this header to depend on the legacy C headers.
        /// Internal: ClockService private data (PIMPL idiom)
        struct Impl;
        boost::scoped_ptr<Impl> impl_;

        friend class Impl;
#ifndef DOXYGEN
        friend class senf::detail::ClockServiceTest;
        friend class singleton<ClockService>;
#endif
    };

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
