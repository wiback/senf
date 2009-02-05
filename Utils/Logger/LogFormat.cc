// $Id$
//
// Copyright (C) 2009 
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
    \brief LogFormat non-inline non-template implementation */

#include "LogFormat.hh"
//#include "LogFormat.ih"

// Custom includes
#include <errno.h>
#include <unistd.h>
#include <locale>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../Scheduler/ClockService.hh"

//#include "LogFormat.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ senf::log::detail::LogFormat::LogFormat()
    : tag_ (detail::getDefaultTag()), noformat_ (false), showTime_ (true),
      showStream_ (false), showLevel_ (true), showArea_ (true), timeBase_ (-1)
{
    std::locale const & loc (datestream_.getloc());
    datestream_.imbue( std::locale(
                           loc, new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%S.%f-0000")) );
}

prefix_ void senf::log::detail::LogFormat::timeFormat(std::string const & format)
{
    if (format.empty()) {
        noformat_ = true;
        timeBase_ = -1;
    } else {
        noformat_ = false;
        std::locale const & loc (datestream_.getloc());
        datestream_.imbue( std::locale(
                               loc, new boost::posix_time::time_facet(format.c_str())) );
    }
}

prefix_ std::string senf::log::detail::LogFormat::prefix(time_type timestamp,
                                                         std::string const & stream,
                                                         std::string const & area,
                                                         unsigned level)
{
    datestream_.str("");

    if (showTime_) {
        if (noformat_) {
            if (timeBase_ == -1) timeBase_ = timestamp;
            time_type delta (timestamp - timeBase_);
            datestream_ << std::setfill('0') << std::setw(10)
                        << (delta / 1000000000ll) << '.'
                        << std::setfill('0') << std::setw(9)
                        << (delta % 1000000000ll);
        }
        else 
            datestream_ << senf::ClockService::abstime(timestamp);
        datestream_ << ' ';
    }
    if (!tag_.empty())
        datestream_ << tag_ << ": ";
    if (showStream_)
        datestream_ << '[' << stream << "] ";
    if (showLevel_)
        datestream_ << '[' << LEVELNAMES[level] << "] ";
    if (showArea_ && area != "senf::log::DefaultArea")
        datestream_ << '[' << area << "] ";

    return datestream_.str();
}

///////////////////////////////////////////////////////////////////////////

prefix_ void senf::log::detail::quoteNonPrintable(std::string & s)
{
    for (std::string::iterator i (s.begin()); i != s.end(); ++i)
        if (*i < ' ' && *i != '\n')
            *i = '?';
}

prefix_ std::string senf::log::detail::getDefaultTag()
{
    std::stringstream ss;
    ss << ::program_invocation_short_name << '[' << ::getpid() << ']';
    return ss.str();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "LogFormat.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
