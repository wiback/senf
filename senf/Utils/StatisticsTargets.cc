// $Id$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.berlios.de/license.html
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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief StatisticsTargets non-inline non-template implementation */

#include "StatisticsTargets.hh"
#include "StatisticsTargets.ih"

// Custom includes
#include <boost/bind.hpp>
#include <senf/Utils/Console/ParsedCommand.hh>
#include "Statistics.hh"

//#include "StatisticsTargets.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::detail::StatisticsLoggerRegistry

prefix_ void
senf::detail::StatisticsLoggerRegistry::apply(StatisticsBase & stats,
                                              unsigned rank, console::DirectoryNode & dir)
{
    Adders::const_iterator i (adders_.begin());
    Adders::const_iterator const i_end (adders_.end());
    for (; i != i_end; ++i)
        (*i)(stats, rank, dir);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    struct RegisterStatisticsLogger
    {
        RegisterStatisticsLogger();

        static void adder(senf::StatisticsBase & stats,
                          unsigned rank, senf::console::DirectoryNode & dir);

        static void consoleCreate(senf::StatisticsBase & stats,
                                  unsigned rank, std::string const & prefix);
    };

    RegisterStatisticsLogger registerStatisticsLogger;
}

prefix_ RegisterStatisticsLogger::RegisterStatisticsLogger()
{
    senf::detail::StatisticsLoggerRegistry::instance().add(&adder);
}

prefix_ void RegisterStatisticsLogger::adder(senf::StatisticsBase & stats,
                                             unsigned rank, senf::console::DirectoryNode & dir)
{
    namespace kw = senf::console::kw;
    namespace fty = senf::console::factory;
    dir.add("logger", fty::Command<void (std::string const &)>(
                boost::bind(&consoleCreate, boost::ref(stats), rank, _1))
            .arg("prefix","Optional prefix string to add to each log message",
                 kw::default_value = "")
            .doc("Send log messages to statistics log stream") );
}

prefix_ void RegisterStatisticsLogger::consoleCreate(senf::StatisticsBase & stats,
                                                     unsigned rank, std::string const & prefix)
{
    stats.output(rank).connect(senf::StatisticsLogger(prefix),
                               "senf::StatisticsLogger(\"" + prefix + "\")");
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
