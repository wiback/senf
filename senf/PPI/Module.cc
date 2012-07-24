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
    \brief Module non-inline non-template implementation */

#include "Module.hh"
#include "Module.ih"

// Custom includes
#include <senf/Utils/algorithm.hh>
#include <senf/Utils/membind.hh>
#include <senf/Utils/Console/ParsedCommand.hh>
#include "Events.hh"
#include "Connectors.hh"

//#include "Module.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::module::Module

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// public members

#ifdef DOXYGEN

prefix_ senf::ppi::Route<connector::InputConnector, connector::OutputConnector> &
senf::ppi::module::Module::route(connector::InputConnector & input,
                                 connector::OutputConnector & output)
{}
prefix_ senf::ppi::Route<connector::InputConnector, EventDescriptor> &
senf::ppi::module::Module::route(connector::InputConnector & input, EventDescriptor & output)
{}

prefix_ senf::ppi::RouteRoute<connector::EventDescriptor, connector::OutputConnector> &
senf::ppi::module::Module::route(EventDescriptor & input, connector::OutputConnector & output);

#else

#define route_impl( Source, Target )                                                        \
    prefix_ senf::ppi::Route<senf::ppi::Source, senf::ppi::Target> &                        \
    senf::ppi::module::Module::route(Source & source, Target & target)                      \
    {                                                                                       \
        detail::RouteHelper<Source,Target>::route(*this, source, target, source, target);   \
        return static_cast< Route<Source,Target> & >(                                       \
                addRoute(std::auto_ptr< RouteBase >(                                        \
                     new Route<Source,Target>(*this, source, target))));                    \
    }

route_impl( connector::GenericPassiveInput, connector::GenericActiveOutput);
route_impl( connector::GenericActiveInput, connector::GenericPassiveOutput);
route_impl( connector::GenericActiveInput, connector::GenericActiveOutput);
route_impl( connector::GenericPassiveInput, connector::GenericPassiveOutput);
route_impl( connector::GenericPassiveInput, EventDescriptor);
route_impl( connector::GenericActiveInput, EventDescriptor);
route_impl( EventDescriptor, connector::GenericPassiveOutput);
route_impl( EventDescriptor, connector::GenericActiveOutput);

#undef route_impl

#endif

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// private members

namespace {
    template <class T>
    std::string consoleDirectoryName(T & t) {
        return senf::prettyName(typeid(t)) + "_" + senf::str(&t);
    }
}

namespace senf { namespace ppi { namespace connector {

    SENF_CONSOLE_REGISTER_ENUM_MEMBER(
        Connector, TraceState, (NO_TRACING)(TRACE_IDS)(TRACE_CONTENTS) );

}}}

prefix_ void senf::ppi::module::Module::registerConnector(connector::Connector & connector)
{
    if (! senf::contains(connectorRegistry_, &connector)) {
        connectorRegistry_.push_back(&connector);
        connector.setModule(*this);
    }
#ifndef SENF_PPI_NOTRACE
    console::DirectoryNode & conDir (console::provideDirectory(
            console::provideDirectory(
                    moduleManager().consoleDir()["modules"], consoleDirectoryName(*this)),
            consoleDirectoryName(connector) ));
    conDir.add("tracing", senf::console::factory::Command(
            SENF_MEMFNP(void, connector::Connector, tracingState, (connector::Connector::TraceState)), &connector));
    conDir.add("tracing", senf::console::factory::Command(
                SENF_MEMFNP(connector::Connector::TraceState, connector::Connector, tracingState, ()), &connector));
#endif
}

prefix_ void senf::ppi::module::Module::unregisterConnector(connector::Connector & connector)
{
    ConnectorRegistry::iterator i (std::find(connectorRegistry_.begin(), connectorRegistry_.end(),
                                             &connector));
    if (i != connectorRegistry_.end())
        connectorRegistry_.erase(i);

    routes_.erase_if(boost::bind(&RouteBase::hasConnector, _1, boost::cref(connector)));

    moduleManager().consoleDir()["modules"][consoleDirectoryName(*this)].remove(
            consoleDirectoryName(connector), senf::nothrow );
}

prefix_ senf::ppi::RouteBase &
senf::ppi::module::Module::addRoute(std::auto_ptr<RouteBase> route)
{
    routes_.push_back(route.release());
    return routes_.back();
}

prefix_ void senf::ppi::module::Module::v_init()
{
    // we can't do this in the constructor,
    // since we want that typeid returns the type of the most derived complete object.
    console::provideDirectory(
            moduleManager().consoleDir()["modules"], consoleDirectoryName(*this));
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
