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
    \brief IOStreamTarget non-inline non-template implementation */

#include "IOStreamTarget.hh"
#include "IOStreamTarget.ih"

// Custom includes
#include <errno.h>
#include <locale>
#include <sstream>
#include <boost/algorithm/string/trim.hpp>
#include <boost/tokenizer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../Scheduler/ClockService.hh"

//#include "IOStreamTarget.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::log::IOStreamTarget

prefix_ senf::log::IOStreamTarget::IOStreamTarget(std::ostream & os)
    : stream_ (os), tag_ (detail::getDefaultTag()), noformat_ (false), showTime_ (true),
      showStream_ (false), showLevel_ (true), showArea_ (true), timeBase_ (-1)
{
    std::locale const & loc (datestream_.getloc());
    datestream_.imbue( std::locale(
                           loc, new boost::posix_time::time_facet("%Y-%m-%d %H:%M:%S.%f-0000")) );
    
}

prefix_ void senf::log::IOStreamTarget::timeFormat(std::string const & format)
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

////////////////////////////////////////
// private members

prefix_ void senf::log::IOStreamTarget::v_write(time_type timestamp,
                                                std::string const & stream,
                                                std::string const & area, unsigned level,
                                                std::string const & message)
{
    std::string m (message);
    boost::trim_right(m);
    detail::quoteNonPrintable(m);

    if (tag_.empty() && !showTime_ && !showStream_ && !showLevel_ && !showArea_)
        stream_ << m << std::endl;
    else {
        typedef boost::char_separator<char> Separator;
        typedef boost::tokenizer<Separator> Tokenizer;
        Separator separator ("\n");
        Tokenizer tokenizer (m, separator);
        Tokenizer::iterator i (tokenizer.begin());
        Tokenizer::iterator const i_end (tokenizer.end());

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

        for (; i != i_end; ++i)
            stream_ << datestream_.str() << *i << "\n";
        stream_ << std::flush;
        datestream_.str("");
    }
}

///////////////////////////////////////////////////////////////////////////

prefix_ void senf::log::detail::quoteNonPrintable(std::string & s)
{
    for (std::string::iterator i (s.begin()); i != s.end(); ++i)
        if (*i < ' ' && *i != '\n')
            *i = ' ';
}

prefix_ std::string senf::log::detail::getDefaultTag()
{
    std::stringstream ss;
    ss << ::program_invocation_short_name << '[' << ::getpid() << ']';
    return ss.str();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "IOStreamTarget.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
