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
    \brief SyslogTarget non-inline non-template implementation */

#include "SyslogTarget.hh"
//#include "SyslogTarget.ih"

// Custom includes
#include <senf/Utils/Console/ParsedCommand.hh>
#include <senf/Utils/Console/Traits.hh>
#include <senf/Utils/Console/ScopedDirectory.hh>

//#include "SyslogTarget.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

int const senf::log::SyslogTarget::LEVELMAP[8] = {
    0, LOG_DEBUG, LOG_INFO, LOG_NOTICE, LOG_WARNING, LOG_CRIT, LOG_EMERG, 0 };

prefix_ void senf::log::SyslogTarget::v_write(time_type timestamp, std::string const & stream,
                                              std::string const & area, unsigned level,
                                              std::string const & message)
{
    if (area != "senf::log::DefaultArea")
        syslog(facility_ | LEVELMAP[level], "[%s] %s", area.c_str(), message.c_str());
    else
        syslog(facility_ | LEVELMAP[level], "%s", message.c_str());
}

namespace senf {
namespace log {

    SENF_CONSOLE_REGISTER_ENUM_MEMBER(SyslogTarget, LogFacility,
                                      (AUTHPRIV)(CRON)(DAEMON)(FTP)(KERN)(LPR)(MAIL)(NEWS)(SYSLOG)
                                      (USER)(UUCP)(LOCAL0)(LOCAL1)(LOCAL2)(LOCAL3)(LOCAL4)(LOCAL5)
                                      (LOCAL6)(LOCAL7));

}}

prefix_ senf::log::SyslogTarget::RegisterConsole::RegisterConsole()
{
    namespace kw = console::kw;
    namespace fty = console::factory;

    detail::TargetRegistry::instance().consoleDir()
        .add("syslog-target",fty::Command(&RegisterConsole::create)
             .arg("facility", "syslog facility to send messages to. One of\n"
                  "                  AUTHPRIV CRON DAEMON FTP KERN LPR MAIL NEWS SYSLOG USER\n"
                  "                  UUCP LOCAL0 LOCAL1 LOCAL2 LOCAL3 LOCAL4 LOCAL5 LOCAL6 LOCAL7",
                  kw::default_value = USER)
             .doc("Create new syslog target. Examples:\n"
                  "\n"
                  "Create new syslog target\n"
                  "    $ syslog-target\n"
                  "    <Directory '/sys/log/syslog'>\n"
                  "\n"
                  "In a configuration file, create new syslog target and set some parameters (If\n"
                  "written on one line, this works at the console too:\n"
                  "    /sys/log/syslog-target LOCAL2 {\n"
                  "        route (IMPORTANT);             # route all important messages\n"
                  "        timeFormat \"\";               # use non-formatted time format\n"
                  "        showArea false;                # don't show log area\n"
                  "    }\n") );
}

prefix_ boost::shared_ptr<senf::console::DirectoryNode>
senf::log::SyslogTarget::RegisterConsole::create(LogFacility facility)
{
    std::unique_ptr<Target> tp (new SyslogTarget(facility));
    Target & target (*tp.get());
    detail::TargetRegistry::instance().dynamicTarget(std::move(tp));
    return target.consoleDir().node().thisptr();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "SyslogTarget.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
