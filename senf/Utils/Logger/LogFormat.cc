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
    \brief LogFormat non-inline non-template implementation */

#include "LogFormat.hh"
//#include "LogFormat.ih"

// Custom includes
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
    : tag_ (detail::getDefaultTag()), timeBase_ (-1), noformat_ (false), showTime_ (true),
      showStream_ (false), showLevel_ (true), showArea_ (true)
{
    timeFormat("%Y-%m-%d %H:%M:%S.%f-0000");
}

prefix_ senf::log::detail::LogFormat::LogFormat(console::ScopedDirectory<> & dir)
    : tag_ (detail::getDefaultTag()), timeBase_ (-1), noformat_ (false), showTime_ (true),
      showStream_ (false), showLevel_ (true), showArea_ (true)
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
            .doc("Set stream display in log messages.") );
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
        timeBase_ = senf::ClockService::in_nanoseconds( ClockService::now());
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
            time_type delta (timestamp - timeBase_);
            datestream_ << std::setfill('0')  << std::right
                        << std::setw(10) << (delta / 1000000000ll) << '.'
                        << std::setw(9) << (delta % 1000000000ll);
        }
        else
            datestream_ << senf::ClockService::abstime( senf::ClockService::nanoseconds(timestamp));
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
