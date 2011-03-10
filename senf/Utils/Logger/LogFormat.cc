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
#include <senf/Scheduler/ClockService.hh>
#include <senf/Utils/Console/ScopedDirectory.hh>
#include <senf/Utils/Console/ParsedCommand.hh>

//#include "LogFormat.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::log::detail::LogFormat::LogFormat()
    : tag_ (detail::getDefaultTag()), noformat_ (false), showTime_ (true),
      showStream_ (false), showLevel_ (true), showArea_ (true), timeBase_ (-1)
{
    timeFormat("%Y-%m-%d %H:%M:%S.%f-0000");
}

prefix_ senf::log::detail::LogFormat::LogFormat(console::ScopedDirectory<> & dir)
    : tag_ (detail::getDefaultTag()), noformat_ (false), showTime_ (true),
      showStream_ (false), showLevel_ (true), showArea_ (true), timeBase_ (-1)
{
    namespace kw = console::kw;
    namespace fty = console::factory;

    timeFormat("%Y-%m-%d %H:%M:%S.%f-0000");

    dir.add("showTime", fty::Command(&LogFormat::showTime, this)
            .arg("flag","whether to display the time in log messages",
                 kw::default_value = true)
            .doc("Set time display in log messages. If time display is enabled, see the 'timeFormat'\n"
                 "command to set the time format.") );
    dir.add("showStream", fty::Command(&LogFormat::showStream, this)
            .arg("flag","whether to display the stream in log messages",
                 kw::default_value = true)
            .doc("Set strean display in log messages.") );
    dir.add("showLevel", fty::Command(&LogFormat::showLevel, this)
            .arg("flag","whether to display the log level in log messages",
                 kw::default_value = true)
            .doc("Set log level display in log messages.") );
    dir.add("showArea", fty::Command(&LogFormat::showArea, this)
            .arg("flag","whether to display the area in log messages",
                 kw::default_value = true)
            .doc("Set area display in log messages.") );
    dir.add("timeFormat", fty::Command(&LogFormat::timeFormat, this)
            .arg("format","time format")
            .doc("Set time format. The time format is specified using a format string. This format\n"
                 "string follows the strftime format.\n"
                 "\n"
                 "As additional option, the format string may be set to the empty string. In this\n"
                 "case the time will be displayed as 'second.nanosecond' value. IN this case, the\n"
                 "time is displayed relative to the first message after changing the format.") );
    dir.add("tag", fty::Command(&LogFormat::tag, this)
            .arg("tag","log message tag prefix")
            .doc("Every log message is optionally prefixed with a tag value. This value defaults to\n"
                 "the executable name and pid.") );
    dir.add("format", fty::Command(&LogFormat::consoleFormat, this)
            .doc("Show the current log message format.") );
}

prefix_ void senf::log::detail::LogFormat::consoleFormat(std::ostream & os)
{
    if (showTime_)                                           os << "showTime ";
    if (showStream_)                                         os << "showStream ";
    if (showLevel_)                                          os << "showLevel ";
    if (showArea_)                                           os << "showArea ";
    if (showTime_ || showStream_ || showLevel_ || showArea_) os << "\n";
    else                                                     os << "(all flags disabled)\n";

    os << "timeFormat \"" << timeFormat_ << "\"\n";
    os << "tag \""        << tag_        << "\"\n";
}

prefix_ void senf::log::detail::LogFormat::timeFormat(std::string const & format)
{
    timeFormat_ = format;
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////

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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
