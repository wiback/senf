// $Id$
//
// Copyright (C) 2008
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
    \brief SyslogUDPTarget non-inline non-template implementation */

#include "SyslogUDPTarget.hh"
//#include "SyslogUDPTarget.ih"

// Custom includes
#include <sstream>
#include <boost/algorithm/string/trim.hpp>
#include <boost/tokenizer.hpp>
#include <senf/Utils/Console/ParsedCommand.hh>
#include <senf/Utils/Console/Traits.hh>
#include <senf/Utils/Console/ScopedDirectory.hh>
#include "SyslogTarget.hh"

//#include "SyslogUDPTarget.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ void senf::log::SyslogUDPTarget::init()
{
    namespace kw = console::kw;
    namespace fty = console::factory;

    consoleDir().remove("format");
    consoleDir()
        .add("format", fty::Command(&SyslogUDPTarget::consoleFormat, this)
             .doc("Show the current log message format.") );
    consoleDir()
        .add("syslog", fty::Command(SENF_MEMBINDFNP(void, SyslogUDPTarget, syslog, (bool)))
             .arg("flag","new syslog format state",
                  kw::default_value=true)
             .doc("Change the syslog format flag. By default, syslog formating is enabled. In this\n"
                  "state, the udp target will send out minimal but valid syslog format messages.\n"
                  "\n"
                  "Disabling syslog format will remove the syslog prefix. Log messages will then be\n"
                  "sent using plain UDP.") );
}

prefix_ void senf::log::SyslogUDPTarget::v_write(time_type timestamp, std::string const & stream,
                                                 std::string const & area, unsigned level,
                                                 std::string const & message)
{
    std::string m (message);
    boost::trim_right(m);
    detail::quoteNonPrintable(m);

    std::stringstream prfstream;
    // The space after the '>' is there on purpose: It ensures, that the prefix (which may be empty)
    // or message will not inadvertently be interpreted as date or hostname by a receiving syslog
    // daemon or proxy
    if (syslogFormat_)
        prfstream << '<' << (facility_ | SyslogTarget::LEVELMAP[level]) << "> ";
    prfstream << prefix(timestamp, stream, area, level);
    std::string const & prf (prfstream.str());

    typedef boost::char_separator<char> Separator;
    typedef boost::tokenizer<Separator> Tokenizer;
    Separator separator ("\n");
    Tokenizer tokenizer (m, separator);
    Tokenizer::iterator i (tokenizer.begin());
    Tokenizer::iterator const i_end (tokenizer.end());

    std::string line;
    unsigned sz (896-prf.size());
    for (; i != i_end; ++i)
        for (unsigned j (0); j < i->size(); j += sz) {
            line = prf;
            line += std::string(*i, j, sz);
            handle_.write(line);
        }
}

prefix_ void senf::log::SyslogUDPTarget::consoleFormat(std::ostream & os)
{
    LogFormat::consoleFormat(os);
    os << "syslog prefix " << (syslogFormat_ ? "enabled" : "disabled") << "\n";
}

namespace senf {
namespace log {

    SENF_CONSOLE_REGISTER_ENUM_MEMBER(SyslogUDPTarget, LogFacility,
                                      (AUTHPRIV)(CRON)(DAEMON)(FTP)(KERN)(LPR)(MAIL)(NEWS)(SYSLOG)
                                      (USER)(UUCP)(LOCAL0)(LOCAL1)(LOCAL2)(LOCAL3)(LOCAL4)(LOCAL5)
                                      (LOCAL6)(LOCAL7));

}}

prefix_ senf::log::SyslogUDPTarget::RegisterConsole::RegisterConsole()
{
    namespace kw = console::kw;
    namespace fty = console::factory;

    detail::TargetRegistry::instance().consoleDir()
        .add("udp-target",
             fty::Command<console::DirectoryNode::ptr (*)(INet4SocketAddress const &,
                                                                LogFacility)
             >(&RegisterConsole::create)
             .arg("address", "target address to send log messages to")
             .arg("facility", "syslog facility to send messages to. One of\n"
                  "                  AUTHPRIV CRON DAEMON FTP KERN LPR MAIL NEWS SYSLOG USER\n"
                  "                  UUCP LOCAL0 LOCAL1 LOCAL2 LOCAL3 LOCAL4 LOCAL5 LOCAL6 LOCAL7",
                  kw::default_value = USER)
             .doc("Create new udp target. The {address} can be an IPv4 or IPv6 address. If the port\n"
                  "number is omitted, it defaults to the default syslog port 514. Examples:\n"
                  "\n"
                  "Create new udp target sending messages to the syslog daemon running at localhost\n"
                  "    $ udp-target localhost\n"
                  "    <Directory '/sys/log/udp-127.0.0.1:514'>\n"
                  "\n"
                  "In a configuration file, create new udp target and set some parameters (If\n"
                  "written on one line, this works at the console too:\n"
                  "    /sys/log/udp-target localhost:2345 LOCAL2 {\n"
                  "        route (IMPORTANT);             # route all important messages\n"
                  "        timeFormat \"\";               # use non-formatted time format\n"
                  "        showArea false;                # don't show log area\n"
                  "        syslog false;                  # no syslog format, just plain udp\n"
                  "    }\n") );
    detail::TargetRegistry::instance().consoleDir()
        .add("udp-target",
             fty::Command<console::DirectoryNode::ptr (*)(INet4Address const &, LogFacility)
             >(&RegisterConsole::create)
             .arg("address")
             .arg("facility", kw::default_value = USER) );
    detail::TargetRegistry::instance().consoleDir()
        .add("udp-target",
             fty::Command<console::DirectoryNode::ptr (*)(INet6SocketAddress const &, LogFacility)
             >(&RegisterConsole::create)
             .arg("address")
             .arg("facility", kw::default_value = USER) );
    detail::TargetRegistry::instance().consoleDir()
        .add("udp-target",
             fty::Command<console::DirectoryNode::ptr (*)(INet6Address const &, LogFacility)
             >(&RegisterConsole::create)
             .arg("address")
             .arg("facility", kw::default_value = USER) );
}

prefix_ boost::shared_ptr<senf::console::DirectoryNode>
senf::log::SyslogUDPTarget::RegisterConsole::create(INet4SocketAddress const & target,
                                                    LogFacility facility)
{
    std::auto_ptr<Target> tp (new SyslogUDPTarget(target, facility));
    Target & tg (*tp.get());
    detail::TargetRegistry::instance().dynamicTarget(move_or_noop(tp));
    return tg.consoleDir().node().thisptr();
}

prefix_ boost::shared_ptr<senf::console::DirectoryNode>
senf::log::SyslogUDPTarget::RegisterConsole::create(INet4Address const & target,
                                                    LogFacility facility)
{
    unique_or_auto_ptr<Target> tp (new SyslogUDPTarget(target, facility));
    Target & tg (*tp.get());
    detail::TargetRegistry::instance().dynamicTarget(move_or_noop(tp));
    return tg.consoleDir().node().thisptr();
}

prefix_ boost::shared_ptr<senf::console::DirectoryNode>
senf::log::SyslogUDPTarget::RegisterConsole::create(INet6SocketAddress const & target,
                                                    LogFacility facility)
{
    unique_or_auto_ptr<Target> tp (new SyslogUDPTarget(target, facility));
    Target & tg (*tp.get());
    detail::TargetRegistry::instance().dynamicTarget(move_or_noop(tp));
    return tg.consoleDir().node().thisptr();
}

prefix_ boost::shared_ptr<senf::console::DirectoryNode>
senf::log::SyslogUDPTarget::RegisterConsole::create(INet6Address const & target,
                                                    LogFacility facility)
{
    unique_or_auto_ptr<Target> tp (new SyslogUDPTarget(target, facility));
    Target & tg (*tp.get());
    detail::TargetRegistry::instance().dynamicTarget(move_or_noop(tp));
    return tg.consoleDir().node().thisptr();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "SyslogUDPTarget.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
