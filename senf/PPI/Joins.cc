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
    \brief Joins non-inline non-template implementation */

#include "Joins.hh"
//#include "Joins.ih"

// Custom includes
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

//#include "Joins.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::ppi::module::PassiveJoin

prefix_ senf::ppi::module::PassiveJoin::PassiveJoin()
{
    noroute(output);
    output.onThrottle(&PassiveJoin::onThrottle);
    output.onUnthrottle(&PassiveJoin::onUnthrottle);
}

////////////////////////////////////////
// private members

prefix_ void senf::ppi::module::PassiveJoin::connectorSetup(connector::PassiveInput<> & conn)
{
    noroute(conn);
    conn.onRequest(boost::bind(&PassiveJoin::request,this,boost::ref(conn)));
    conn.qdisc( QueueingDiscipline::NONE);
}

prefix_ void senf::ppi::module::PassiveJoin::onThrottle()
{
    using boost::lambda::_1;
    using boost::lambda::bind;
    std::for_each(connectors().begin(), connectors().end(),
                  bind(&connector::GenericPassiveInput::throttle, _1));
}

prefix_ void senf::ppi::module::PassiveJoin::onUnthrottle()
{
    using boost::lambda::_1;
    using boost::lambda::bind;
    std::for_each(connectors().begin(), connectors().end(),
                  bind(&connector::GenericPassiveInput::unthrottle, _1));
}

///////////////////////////////////////////////////////////////////////////
// senf::ppi::module::PriorityJoin

prefix_ senf::ppi::module::PriorityJoin::PriorityJoin()
{
    noroute(output);
    output.onRequest(&PriorityJoin::request);
}

////////////////////////////////////////
// private members

prefix_ void
senf::ppi::module::PriorityJoin::connectorSetup(PriorityJoin::ConnectorType & conn,
                                                int priority)
{
    noroute(conn);
    conn.onThrottle(&PriorityJoin::onThrottle);
    conn.onUnthrottle(&PriorityJoin::onUnthrottle);

    if (priority < 0) {
        priority = connectors().size() + priority;
        if (priority < 0)
            priority = 0;
    }
    if (priority >= int(connectors().size())-1)
        return;

    connectors().insert(connectors().begin()+priority, connectors().pop_back().release());
}

prefix_ void senf::ppi::module::PriorityJoin::request()
{
    using boost::lambda::_1;
    using boost::lambda::bind;
    PriorityJoin::ContainerType::iterator i (
        std::find_if(connectors().begin(), connectors().end(),
                     ! bind(&connector::GenericActiveInput::throttled, _1)));
    if (i != connectors().end())
        output((*i)());
}

prefix_ void senf::ppi::module::PriorityJoin::onThrottle()
{
    if (std::find_if(connectors().begin(), connectors().end(),
                     ! bind(&connector::GenericActiveInput::throttled, _1)) == connectors().end())
        output.throttle();
}

prefix_ void senf::ppi::module::PriorityJoin::onUnthrottle()
{
    output.unthrottle();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "Joins.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
