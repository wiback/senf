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
    \brief Module non-inline non-template implementation */

#include "Module.hh"
#include "Module.ih"

// Custom includes
#include <senf/Utils/algorithm.hh>
#include <senf/Utils/String.hh>
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
                addRoute(std::unique_ptr< RouteBase >(                                       \
                     new Route<Source,Target>(source, target))));                           \
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
        return senf::prettyBaseName(typeid(t)) + "_" + senf::str(&t);
    }
}

prefix_ void senf::ppi::module::Module::registerConnector(connector::Connector & connector)
{
    if (! senf::contains(connectorRegistry_, &connector)) {
        connectorRegistry_.push_back(&connector);
        connector.setModule(*this);
        sysConsoleDir().add( consoleDirectoryName(connector), connector.consoleDir())
                .doc( senf::prettyName(typeid(connector)) + " " + senf::str(&connector));
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
senf::ppi::module::Module::addRoute(std::unique_ptr<RouteBase> route)
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

prefix_ void senf::ppi::module::Module::v_init()
{
    // we can't do this in the constructor,
    // since we want that typeid returns the type of the most derived complete object.
    moduleManager().consoleDir()["modules"].add( consoleDirectoryName(*this), sysConsoleDir_)
            .doc( senf::prettyName(typeid(*this)) + " " + senf::str(this));
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
