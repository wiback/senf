//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief EventManager non-inline non-template implementation */

//#include "EventManager.hh"
//#include "EventManager.ih"

// Custom includes
#include <boost/format.hpp>
#include <senf/Utils/membind.hh>
#include <senf/Utils/Console/ScopedDirectory.hh>
#include <senf/Utils/Console/ParsedCommand.hh>
#include "ConsoleDir.hh"
#include "FIFORunner.hh"

//#include "EventManager.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ senf::scheduler::detail::EventManager::EventManager()
{
#ifndef SENF_DISABLE_CONSOLE
    consoleDir().add("events", console::factory::Command(
            membind(&EventManager::listEvents, this))
        .doc("List all scheduler events sorted by priority\n"
             "\n"
             "Columns:\n"
             "    TP      event type:\n"
             "              fd  file descriptor\n"
             "              tm  timer\n"
             "              si  UNIX signal\n"
             "              ee  event hook\n"
             "    NAME    descriptive event name\n"
             "    ADDRESS address of event class instance\n"
             "    RUNCNT  number of times, the event was called\n"
             "    S       state:\n"
             "              R  runnable\n"
             "              W  waiting\n"
             "              -  event disabled\n"
             "    INFO    further event specific information")
        );
#endif
}

prefix_ void senf::scheduler::detail::EventManager::listEvents(std::ostream & os)
{
    // On an 80 column display, this wraps nicely directly before the INFO column
    boost::format fmt  ("%s %-55.55s 0x%016x %8d %s %s\n");
    os << boost::format("%s %-55.55s %-18s %8s %s %s\n")
        % "TP" % "NAME" % "ADDRESS" % "RUNCNT" % "S" % "INFO";
    {
        FIFORunner::iterator i (FIFORunner::instance().begin());
        FIFORunner::iterator const i_end (FIFORunner::instance().end());
        for (; i != i_end; ++i)
            os << fmt
                % i->type()
                % i->name()
                % reinterpret_cast<unsigned long>(&(*i))
                % i->runCount()
                % (i->runnable() ? "R" : "W")
                % i->info();
    } {
        iterator i (begin());
        iterator const i_end (end());
        for (; i != i_end; ++i)
            if (! i->enabled())
                os << fmt
                    % i->type()
                    % i->name()
                    % reinterpret_cast<unsigned long>(&(*i))
                    % i->runCount()
                    % "-"
                    % i->info();
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "EventManager.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
