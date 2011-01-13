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
    \brief Module non-inline non-template implementation */

#include "Module.hh"
#include "Module.ih"

// Custom includes

//#include "Module.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::module::Module

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// private members

prefix_ void senf::ppi::module::Module::registerConnector(connector::Connector & connector)
{
    if (std::find(connectorRegistry_.begin(), connectorRegistry_.end(), &connector)
        == connectorRegistry_.end()) {
        connectorRegistry_.push_back(&connector);
        connector.setModule(*this);
    }
}

prefix_ void senf::ppi::module::Module::unregisterConnector(connector::Connector & connector)
{
    ConnectorRegistry::iterator i (std::find(connectorRegistry_.begin(), connectorRegistry_.end(),
                                             &connector));
    if (i != connectorRegistry_.end())
        connectorRegistry_.erase(i);

    routes_.erase_if(boost::bind(&RouteBase::hasConnector, _1, boost::cref(connector)));
}

prefix_ senf::ppi::RouteBase &
senf::ppi::module::Module::addRoute(std::auto_ptr<RouteBase> route)
{
    routes_.push_back(route.release());
    return routes_.back();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// protected members

prefix_ void senf::ppi::module::Module::noroute(connector::Connector & connector)
{
    registerConnector(connector);
    connector.setModule(*this);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "Module.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
