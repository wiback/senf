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
    if (SENF_LIKELY(i != connectors().end()))
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
