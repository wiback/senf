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
