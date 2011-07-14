// $Id$
//
// Copyright (C) 2007
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
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::module::PassiveJoin

prefix_ senf::ppi::module::PassiveJoin::PassiveJoin()
{
    noroute(output);
    output.onThrottle(&PassiveJoin::onThrottle);
    output.onUnthrottle(&PassiveJoin::onUnthrottle);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::module::PriorityJoin

prefix_ senf::ppi::module::PriorityJoin::PriorityJoin()
{
    noroute(output);
    output.onRequest(&PriorityJoin::request);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
