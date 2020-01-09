// $Id$
//
// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fit.fraunhofer.de/license.html
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
//   Stefan Bund <senf@g0dil.de>

/** \file
    \brief ClockService non-inline non-template implementation */

#include "ClockService.hh"
//#include "ClockService.ih"

// Custom includes
#include <boost/regex.hpp>
#include <senf/Utils/Console/Traits.hh>

// this include needed to add dependency on Scheduler. This should really be in 
// ClockService.cci which is not possible due to recursive includes ...
#include <senf/Scheduler/Scheduler.hh>

#define prefix_

#ifdef SENF_DEBUG
#   define SENF_CLOCKTYPEVAL(clock) (clock).value()
#else
#   define SENF_CLOCKTYPEVAL(clock) (clock)
#endif

//-/////////////////////////////////////////////////////////////////////////////////////////////////

template <class T> static senf::ClockService::clock_type
parseClockServiceInterval__(std::string const & value)
{
    senf::ClockService::clock_type out (senf::ClockService::clock_type(0));

    static boost::sregex_iterator::regex_type rx ("[mun]?[dhms]");
    boost::sregex_iterator i (value.begin(), value.end(), rx);
    boost::sregex_iterator const i_end;
    std::string::const_iterator j (value.begin());
    for (; i != i_end; ++i) {
        boost::sregex_iterator::value_type::value_type match ((*i)[0]);
        T v (boost::lexical_cast<T>(std::string(j, match.first)));
        char scale (0);
        char unit (0);
        if (match.length() == 2) {
            scale = *match.first;
            unit = *boost::next(match.first);
        } else {
            SENF_ASSERT( match.length() == 1,
                         "Internal failure: RegEx match returns weird number of matches" );
            unit = *match.first;
        }
        switch (unit) {
        case 'd': v *= 24;  // fall through
        case 'h': v *= 60;  // fall through
        case 'm': v *= 60;  // fall through
        case 's': v *= 1000000000;
        }
        switch (scale) {
        case 0: break;
        case 'n': v /= 1000;  // fall through
        case 'u': v /= 1000;  // fall through
        case 'm': v /= 1000;
        }
        out += senf::ClockService::nanoseconds(senf::ClockService::int64_type(v));
        j = match.second;
    }
    if (j != value.end())
        throw senf::console::SyntaxErrorException();

    return out;
}

prefix_ void
senf::parseClockServiceInterval(console::ParseCommandInfo::TokensRange const & tokens,
                                ClockService::clock_type & out)
{
    out = senf::ClockService::clock_type(0);
    std::string value;
    {
        console::CheckedArgumentIteratorWrapper arg (tokens);
        console::parse( *(arg++), value );
    }

    // only use the double parser for float values
    if (value.find('.') != std::string::npos) {
        out = parseClockServiceInterval__<long double>(value);
    } else {
        out = parseClockServiceInterval__<ClockService::int64_type>(value);
    }
}

prefix_ senf::ClockService::clock_type senf::ClockService::clock_m(abstime_type const & time)
{
    if (scheduler::now() - baseClock_ > clock_type(1000000000ll))
        restart_m();
    boost::posix_time::time_duration delta (time - baseAbstime_);
    return baseClock_ + clock_type( delta.ticks() )
        * clock_type( 1000000000UL / boost::posix_time::time_duration::ticks_per_second() );
}

prefix_ senf::ClockService::abstime_type senf::ClockService::abstime_m(clock_type const & clock)
{
    if (clock == clock_type(0))
        return abstime_type();
    if (scheduler::now() - baseClock_ > clock_type(1000000000ll))
        restart_m();
#ifdef BOOST_DATE_TIME_POSIX_TIME_STD_CONFIG
    return baseAbstime_ + boost::posix_time::nanoseconds(
            SENF_CLOCKTYPEVAL( clock-baseClock_));
#else
    return baseAbstime_ + boost::posix_time::microseconds(
            SENF_CLOCKTYPEVAL( (clock-baseClock_+clock_type(500))/1000));
#endif
    return abstime_type();
}

prefix_ void senf::formatClockServiceInterval(ClockService::clock_type interval,
                                              std::ostream & os)
{
    os << interval << "ns";
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "ClockService.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
