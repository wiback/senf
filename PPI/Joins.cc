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

////////////////////////////////////////
// private members

prefix_ senf::ppi::connector::PassiveInput<> & senf::ppi::module::PassiveJoin::newInput()
{
    inputs_.push_back(new connector::PassiveInput<>());
    connector::PassiveInput<> & input (inputs_.back());

    noroute(input);
    input.onRequest(boost::bind(&PassiveJoin::request,this,boost::ref(input)));

    return input;
}

prefix_ void senf::ppi::module::PassiveJoin::request(connector::GenericPassiveInput & input)
{
    output(input());
}

prefix_ void senf::ppi::module::PassiveJoin::onThrottle()
{
    using boost::lambda::_1;
    using boost::lambda::bind;
    std::for_each(inputs_.begin(), inputs_.end(),
                  bind(&connector::GenericPassiveInput::throttle, _1));
}

prefix_ void senf::ppi::module::PassiveJoin::onUnthrottle()
{
    using boost::lambda::_1;
    using boost::lambda::bind;
    std::for_each(inputs_.begin(), inputs_.end(),
                  bind(&connector::GenericPassiveInput::unthrottle, _1));
}

///////////////////////////////////////////////////////////////////////////
// senf::ppi::module::PriorityJoin

////////////////////////////////////////
// private members

prefix_ senf::ppi::connector::ActiveInput<> & senf::ppi::module::PriorityJoin::newInput()
{
    inputs_.push_back(new connector::ActiveInput<>());
    connector::ActiveInput<> & input (inputs_.back());

    noroute(input);
    input.onThrottle(&PriorityJoin::onThrottle);
    input.onUnthrottle(&PriorityJoin::onUnthrottle);

    return input;
}

prefix_ void senf::ppi::module::PriorityJoin::request()
{
    using boost::lambda::_1;
    using boost::lambda::bind;
    Inputs::iterator i (std::find_if(inputs_.begin(), inputs_.end(),
                                     ! bind(&connector::GenericActiveInput::throttled, _1)));
    if (i != inputs_.end())
        output((*i)());
}

prefix_ void senf::ppi::module::PriorityJoin::onThrottle()
{
    if (std::find_if(inputs_.begin(), inputs_.end(),
                     ! bind(&connector::GenericActiveInput::throttled, _1)) == inputs_.end())
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
