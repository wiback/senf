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
#include "ConsoleTarget.hh"

//#include "Target.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::log::Target

prefix_ senf::log::Target::Target()
{
    detail::TargetRegistry::instance().registerTarget(this);
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
    detail::StreamBase const * s (StreamRegistry::instance().lookup(stream));
    if (!s)
        throw InvalidStreamException();
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
    detail::StreamBase const * s (StreamRegistry::instance().lookup(stream));
    if (!s)
        throw InvalidStreamException();
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

///////////////////////////////////////////////////////////////////////////
// senf::log::detail::TargetRegistry

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
///////////////////////////////////////////////////////////////////////////
// namespace members

prefix_ std::ostream & senf::log::operator<<(std::ostream & os, senf::log::Target::action_t const & action)
{
    if( action == Target::ACCEPT) os << "ACCEPT";
    else if( action == Target::REJECT) os << "REJECT";
    else os << "unknown action";
    return os;
}


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
