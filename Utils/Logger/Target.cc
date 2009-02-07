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
    \brief Target non-inline non-template implementation */

#include "Target.hh"
#include "Target.ih"

// Custom includes
#include <algorithm>
#include <boost/format.hpp>
#include "ConsoleTarget.hh"
#include "../Console/Console.hh"
#include "../Console/Sysdir.hh"
#include "../membind.hh"

//#include "Target.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::log::Target

namespace senf {
namespace log {

    SENF_CONSOLE_REGISTER_ENUM_MEMBER( Target, action_t, (ACCEPT)(REJECT) );

namespace detail {

    SENF_CONSOLE_REGISTER_ENUM_MEMBER( TargetRegistry, Level, 
                                       (VERBOSE)(NOTICE)(MESSAGE)(IMPORTANT)(CRITICAL)(FATAL) );

}}}

prefix_ senf::log::Target::Target(std::string const & name)
{
    namespace kw = senf::console::kw;

    detail::TargetRegistry::instance().registerTarget(this, name);
    consoleDir_().add("list", senf::membind(&Target::consoleList, this))
        .doc("Show routing table\n"
             "\n"
             "Columns:\n"
             "    #       rule index\n"
             "    STREAM  stream to match, empty to match all streams\n"
             "    AREA    area to match, empty to match all targets\n"
             "    LEVEL   match messages with level above this. Log levels in increasing order\n"
             "            are:\n"
             "                verbose, notice, message, important, critical, fatal\n"
             "    ACTION  action to take: accept or reject");
    consoleDir_().add("route", 
                      boost::function<void (std::string const &, std::string const &,
                                            detail::TargetRegistry::Level, action_t, int)>(
                                                senf::membind(
                                                    static_cast<void (Target::*)(
                                                        std::string const &, std::string const &,
                                                        unsigned, action_t, int)>(&Target::route),
                                                    this)))
        .arg("stream", 
             "stream to match or empty to match any stream\n"
             "              use '/sys/log/streams' to list all available streams",
             kw::default_value="")
        .arg("area", 
             "area to match or empty to match any area\n"
             "              use '/sys/log/areas' to list all available areas",
             kw::default_value="")
        .arg("level", "log level, one of: VERBOSE, NOTICE, MESSAGE, IMPORTANT, CRITICAL, FATAL",
             kw::default_value=detail::TargetRegistry::VERBOSE)
        .arg("action", "routing action, one of: ACCEPT, REJECT",
             kw::default_value=ACCEPT)
        .arg("index", "index at which to insert new rule",
             kw::default_value=-1)
        .doc("Add routing entry. Log messages are matched against the routing table beginning\n"
             "with the first entry. The action of the first matching entry determines the\n"
             "handling of the message.\n"
             "\n"
             "Examples:\n"
             "\n"
             "    route\n"
             "        route all messages to this target.\n"
             "\n"
             "    route \"\" my::Class\n"
             "        route all messages which are in the my::Class area.\n"
             "\n"
             "    route senf::log::Debug \"\" VERBOSE REJECT\n"
             "    route \"\" \"\" VERBOSE\n"
             "        route all messages not in the senf::log::Debug stream to the current area.\n"
             "\n"
             "The additional optional index argument identifies the position in the routing table\n"
             "where the new routing entry will be added. Positive numbers count from the\n"
             "beginning, 0 being the first routing entry. Negative values count from the end.\n");
    consoleDir_().add("unroute",
                      senf::membind(static_cast<void (Target::*)(int)>(&Target::unroute), this))
        .arg("index", "index of routing entry to remove")
        .overloadDoc("Remove routing entry with the given index");
    consoleDir_().add("unroute", 
                      boost::function<void (std::string const &, std::string const &,
                                            detail::TargetRegistry::Level, action_t)>(
                                                senf::membind(
                                                    static_cast<void (Target::*)(
                                                        std::string const &, std::string const &,
                                                        unsigned, action_t)>(&Target::unroute),
                                                    this)))
        .arg("stream", "stream to match or empty to match any stream",
             kw::default_value="")
        .arg("area", "area to match or empty to match any area",
             kw::default_value="")
        .arg("level", "log level, one of: VERBOSE, NOTICE, MESSAGE, IMPORTANT, CRITICAL, FATAL",
             kw::default_value=detail::TargetRegistry::VERBOSE)
        .arg("action", "routing action, one of: ACCEPT, REJECT",
             kw::default_value=ACCEPT)
        .overloadDoc("Remove the routing entry matching the specified arguments.");
    consoleDir_().add("flush", senf::membind(&Target::flush, this))
        .doc("Remove all routing entries clearing the routing table. This will disable all\n"
             "logging output on this target.");
}

prefix_ senf::log::Target::~Target()
{
    while( ! rib_.empty()) {
        // This is slow but simplifies the area cache handling and removing a target should be
        // relatively seldom
        RIB::reverse_iterator i (rib_.rbegin());
        unroute(i->stream_, i->area_, i->level_, i->action_);
    }
    detail::TargetRegistry::instance().unregisterTarget(this);
}

prefix_ void senf::log::Target::route(std::string const & stream, std::string const & area,
                                      unsigned level, action_t action, int index)
{
    detail::StreamBase const * s (0);
    if (! stream.empty()) {
        s = StreamRegistry::instance().lookup(stream);
        if (!s)
            throw InvalidStreamException();
    }
    detail::AreaBase const * a (0);
    if (! area.empty()) {
        a = AreaRegistry::instance().lookup(area);
        if (!a)
            throw InvalidAreaException();
    }
    route(s, a, level, action, index);
}

prefix_ void senf::log::Target::unroute(std::string const & stream, std::string const & area,
                                        unsigned level, action_t action)
{
    detail::StreamBase const * s (0);
    if (! stream.empty()) {
        s = StreamRegistry::instance().lookup(stream);
        if (!s)
            throw InvalidStreamException();
    }
    detail::AreaBase const * a (0);
    if (! area.empty()) {
        a = AreaRegistry::instance().lookup(area);
        if (!a)
            throw InvalidAreaException();
    }
    unroute(s, a, level, action);
}

prefix_ void senf::log::Target::unroute(int index)
{
    if (rib_.empty())
        return;
    RIB::iterator i;
    if (index < 0) {
        if (RIB::size_type(-index) >= rib_.size())
            i = rib_.begin();
        else {
            i = rib_.end();
            std::advance(i, index);
        }
    } else {
        if (RIB::size_type(index+1) >= rib_.size()) {
            i = rib_.end();
            --i;
        } else {
            i = rib_.begin();
            std::advance(i, index);
        }
    }
    if (i == rib_.end())
        return;
    RoutingEntry entry (*i);
    rib_.erase(i);
    if (entry.action_ == ACCEPT)
        updateRoutingCache(entry.stream_, entry.area_);
}

prefix_ void senf::log::Target::flush()
{
    RIB old;
    rib_.swap(old);
    RIB::const_iterator i (old.begin());
    RIB::const_iterator const i_end (old.end());
    for (; i != i_end; ++i)
        if (i->action_ == ACCEPT)
            updateRoutingCache(i->stream_, i->area_);
}

////////////////////////////////////////
// protected members

prefix_ senf::log::detail::TargetRegistry::TargetRegistry()
    : fallbackRouting_(true)
{
    namespace kw = senf::console::kw;

    console::sysdir().add("log", consoleDir_());
    consoleDir_().add("areas", senf::membind(&TargetRegistry::consoleAreas, this))
        .doc("List all areas");
    consoleDir_().add("streams", senf::membind(&TargetRegistry::consoleStreams, this))
        .doc("List all streams");
    consoleDir_().add("message", senf::membind(&TargetRegistry::consoleWrite, this))
        .arg("stream", "stream to write message to",
             kw::default_value = "senf::log::Debug")
        .arg("area","area to write message to",
             kw::default_value = "senf::log::DefaultArea")
        .arg("level", "log level, one of:\n"
             "                  VERBOSE, NOTICE, MESSAGE, IMPORTANT, CRITICAL, FATAL",
             kw::default_value = MESSAGE)
        .arg("message", "message to write")
        .doc("Write log message");
}

prefix_ senf::log::detail::TargetRegistry::~TargetRegistry()
{
    Targets::iterator i (dynamicTargets_.begin());
    Targets::iterator const i_end (dynamicTargets_.end());
    for (; i != i_end; ++i)
        delete *i;
}

prefix_ void senf::log::detail::TargetRegistry::consoleAreas(std::ostream & os)
{
    AreaRegistry::iterator i (AreaRegistry::instance().begin());
    AreaRegistry::iterator const i_end (AreaRegistry::instance().end());
    for (; i != i_end; ++i)
        os << *i << "\n";
}

prefix_ void senf::log::detail::TargetRegistry::consoleStreams(std::ostream & os)
{
    StreamRegistry::iterator i (StreamRegistry::instance().begin());
    StreamRegistry::iterator const i_end (StreamRegistry::instance().end());
    for (; i != i_end; ++i)
        os << *i << "\n";
}

prefix_ void senf::log::detail::TargetRegistry::consoleWrite(std::string const & stream,
                                                             std::string const & area,
                                                             Level level,
                                                             std::string const & msg)
{
    detail::StreamBase const * s (StreamRegistry::instance().lookup(stream));
    if (!s)
        throw Target::InvalidStreamException();
    detail::AreaBase const * a (AreaRegistry::instance().lookup(area));
    if (!a)
        throw Target::InvalidAreaException();
    write(*s, *a, level, msg);
}

prefix_ senf::console::ScopedDirectory<> & senf::log::Target::consoleDir()
{
    return consoleDir_();
}

////////////////////////////////////////
// private members

prefix_ void senf::log::Target::route(detail::StreamBase const * stream,
                                      detail::AreaBase const * area, unsigned level,
                                      action_t action, int index)
{
    RIB::iterator i;
    if (index < 0) {
        if (RIB::size_type(-index-1) >= rib_.size())
            i = rib_.begin();
        else {
            i = rib_.end();
            std::advance(i, index + 1 );
        }
    } else {
        if (RIB::size_type(index) >= rib_.size())
            i = rib_.end();
        else {
            i = rib_.begin();
            std::advance(i, index);
        }
    }
    rib_.insert(i, RoutingEntry(stream, area, level, action));
    if (action == ACCEPT)
        updateRoutingCache(stream, area);
    // This disables the initial fallback routing
    detail::TargetRegistry::instance().routed();
}

prefix_ void senf::log::Target::unroute(detail::StreamBase const * stream,
                                        detail::AreaBase const * area, unsigned level,
                                        action_t action)
{
    RIB::iterator i = std::find(rib_.begin(), rib_.end(),
                                RoutingEntry(stream, area, level, action));
    if (i != rib_.end())
        unroute(std::distance(rib_.begin(), i));
}

prefix_ void senf::log::Target::updateRoutingCache(detail::StreamBase const * stream,
                                                   detail::AreaBase const * area)
{
    if (! stream) {
        StreamRegistry::Registry::iterator i (StreamRegistry::instance().registry_.begin());
        StreamRegistry::Registry::iterator const i_end (StreamRegistry::instance().registry_.end());
        for (; i != i_end ; ++i)
            updateRoutingCache(i->second, area);
        return;
    }
    if (! area) {
        AreaRegistry::Registry::iterator i (AreaRegistry::instance().registry_.begin());
        AreaRegistry::Registry::iterator const i_end (AreaRegistry::instance().registry_.end());
        for (; i != i_end ; ++i)
            updateRoutingCache(stream, i->second);
        return;
    }
    unsigned limit (DISABLED::value);
    RIB::iterator i (rib_.begin());
    RIB::iterator const i_end (rib_.end());
    for(; i != i_end; ++i)
        if ( (! i->stream_ || i->stream_ == stream) &&
             (! i->area_ || i->area_ == area) &&
             i->action_ == ACCEPT ) {
            unsigned l (i->level_ == NONE::value ? stream->defaultRuntimeLimit() : i->level_);
            if (l < limit)
                limit = l;
        }
    if (limit == DISABLED::value)
        area->removeRoutingCache(*this, *stream);
    else
        area->updateRoutingCache(*this, *stream, limit);
}

prefix_ void senf::log::Target::write(time_type timestamp,
                                      detail::StreamBase const & stream,
                                      detail::AreaBase const & area, unsigned level,
                                      std::string const & message)
{
    RIB::iterator i (rib_.begin());
    RIB::iterator const i_end (rib_.end());
    for (; i != i_end; ++i)
        if ( (! i->stream_ || i->stream_ == &stream) &&
             (! i->area_ || i->area_ == &area) &&
             (i->level_ == NONE::value ? stream.defaultRuntimeLimit() : i->level_) <= level ) {
            if (i->action_ == ACCEPT)
                v_write(timestamp, stream.v_name(), area.v_name(), level, message);
            return;
        }
}

namespace {
    std::string formatLabel(std::string const & l)
    {
        if (l.empty())
            return "-";
        if (l.size() > 29)
            return l.substr(l.size()-29);
        return l;
    }
}

prefix_ void senf::log::Target::consoleList(std::ostream & os)
{
    static char const * levels[] = { 
        "none", "verbose", "notice", "message", "important", "critical", "fatal", "disabled" };

    boost::format fmt ("%2d %-29s %-29s %-9s %-6s\n");
    os << fmt % "#" % "STREAM" % "AREA" % "LEVEL" % "ACTION";
    unsigned n (0);
    for (iterator i (begin()); i != end(); ++i, ++n)
        os << fmt 
            % n 
            % formatLabel(i->stream())
            % formatLabel(i->area())
            % levels[i->level()]
            % (i->action() == ACCEPT ? "accept" : "reject");
}

///////////////////////////////////////////////////////////////////////////
// senf::log::detail::TargetRegistry

prefix_ void senf::log::detail::TargetRegistry::dynamicTarget(std::auto_ptr<Target> target)
{
    target->consoleDir().add("remove", boost::function<void ()>(
                                 boost::bind(
                                     &TargetRegistry::consoleRemoveTarget, this, target.get())))
        .doc("Remove target.");
    dynamicTargets_.insert(target.release());
}

prefix_ void senf::log::detail::TargetRegistry::registerTarget(Target * target,
                                                               std::string const & name)
{
    targets_.insert(target);
    consoleDir_().add(name, target->consoleDir_());
}

prefix_ void senf::log::detail::TargetRegistry::write(StreamBase const & stream,
                                                      AreaBase const & area, unsigned level,
                                                      std::string const & msg)
{
    if (fallbackRouting_) {
        if (level >= stream.defaultRuntimeLimit())
            static_cast<Target &>(ConsoleTarget::instance()).v_write(
                TimeSource::now(), stream.v_name(), area.v_name(), level, msg );
    }
    else
        area.write( TimeSource::now(), stream, level, msg );
}

prefix_ void senf::log::detail::TargetRegistry::consoleRemoveTarget(Target * target)
{
    dynamicTargets_.erase(target);
    delete target;
}

///////////////////////////////////////////////////////////////////////////
// namespace members

prefix_ std::ostream & senf::log::operator<<(std::ostream & os, senf::log::Target::action_t const & action)
{
    if( action == Target::ACCEPT) os << "ACCEPT";
    else if( action == Target::REJECT) os << "REJECT";
    else os << "unknown action";
    return os;
}

//////////////////////////////////////////////////////////////////////////
// I need to put this here, otherwise the file target will not be registered
// if it is not used ... :-(

senf::log::FileTarget::RegisterConsole senf::log::FileTarget::RegisterConsole::instance;
senf::log::SyslogTarget::RegisterConsole senf::log::SyslogTarget::RegisterConsole::instance;
senf::log::SyslogUDPTarget::RegisterConsole senf::log::SyslogUDPTarget::RegisterConsole::instance;

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "Target.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
