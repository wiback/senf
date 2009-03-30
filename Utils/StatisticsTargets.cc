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
    \brief StatisticsTargets non-inline non-template implementation */

#include "StatisticsTargets.hh"
#include "StatisticsTargets.ih"

// Custom includes
#include <boost/bind.hpp>
#include "Console/Console.hh"
#include "Statistics.hh"

//#include "StatisticsTargets.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::detail::StatisticsLoggerRegistry

prefix_ void
senf::detail::StatisticsLoggerRegistry::apply(senf::StatisticsBase & stats,
                                              unsigned rank,
                                              senf::console::DirectoryNode & dir)
{
    Adders::const_iterator i (adders_.begin());
    Adders::const_iterator const i_end (adders_.end());
    for (; i != i_end; ++i)
        (*i)(stats, rank, dir);
}

///////////////////////////////////////////////////////////////////////////

namespace {

    struct RegisterStatisticsLogger
    {
        RegisterStatisticsLogger();

        static void adder(senf::StatisticsBase & stats, 
                          unsigned rank,
                          senf::console::DirectoryNode & dir);

        static void consoleCreate(senf::StatisticsBase & stats, 
                                  unsigned rank,
                                  std::string const & prefix);
    };

    RegisterStatisticsLogger registerStatisticsLogger;
}

prefix_ RegisterStatisticsLogger::RegisterStatisticsLogger()
{
    senf::detail::StatisticsLoggerRegistry::instance().add(&adder);
}

prefix_ void RegisterStatisticsLogger::adder(senf::StatisticsBase & stats,
                                             unsigned rank,
                                             senf::console::DirectoryNode & dir)
{
    namespace kw = senf::console::kw;
    
    dir.add("logger", boost::function<void (std::string const &)>(
               boost::bind(&consoleCreate, boost::ref(stats), rank, _1)))
       .arg("prefix","Optional prefix string to add to each log message",
            kw::default_value = "")
       .doc("Send log messages to statistics log stream");
}

prefix_ void RegisterStatisticsLogger::consoleCreate(senf::StatisticsBase & stats,
                                                     unsigned rank,
                                                     std::string const & prefix)
{
    stats.output(rank).connect(senf::StatisticsLogger(prefix),
                               "senf::StatisticsLogger(\"" + prefix + "\")");
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "StatisticsTargets.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
