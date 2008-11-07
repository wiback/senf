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
    \brief Connectors non-inline non-template implementation */

#include "Connectors.hh"
#include "Connectors.ih"

// Custom includes
#include "Route.hh"
#include "Module.hh"
#include "ModuleManager.hh"

//#include "Connectors.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::Connector

prefix_ void senf::ppi::connector::Connector::connect(Connector & target)
{
    // The connector is not registered -> route() or noroute() statement missing
    SENF_ASSERT( module_ );
    // The connector is already connected
    SENF_ASSERT( ! peer_ );
    // The target connector is not registered -> route() or noroute() statement missing
    SENF_ASSERT( target.module_ );
    // The target connector is already connected
    SENF_ASSERT( ! target.peer_ );
    if (! (packetTypeID() == typeid(void) ||
           target.packetTypeID() == typeid(void) || 
           packetTypeID() == target.packetTypeID()) )
        throw IncompatibleConnectorsException() 
            << ": " << prettyName(packetTypeID()) 
            << " [in module " << prettyName(typeid(*module_))  << "] "
            << ", " << prettyName(target.packetTypeID())
            << " [in module " << prettyName(typeid(*target.module_)) << "]";
            
    peer_ = & target;
    target.peer_ = this;

    if (! initializationScheduled())
        enqueueInitializable();
    if (! peer().initializationScheduled())
        peer().enqueueInitializable();
}

prefix_ void senf::ppi::connector::Connector::disconnect()
{
    // Cannot disconnected a non-connected connector
    SENF_ASSERT( peer_ );
    Connector & peer (*peer_);
    peer_ = 0;
    peer.peer_ = 0;

    if (! initializationScheduled())
        enqueueInitializable();
    if (! peer.initializationScheduled())
        peer.enqueueInitializable();
}

prefix_ std::type_info const & senf::ppi::connector::Connector::packetTypeID()
{
    return typeid(void);
}

///////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::PassiveConnector

////////////////////////////////////////
// private members

prefix_ void senf::ppi::connector::PassiveConnector::v_init()
{
    Routes::const_iterator i (routes_.begin());
    Routes::const_iterator const i_end (routes_.end());
    for (; i != i_end; ++i)
        if ((*i)->throttled())
            break;
    if (i == i_end)
        remoteThrottled_ = false;
    if (throttled())
        emitThrottle();
    else
        emitUnthrottle();
}

prefix_ void senf::ppi::connector::PassiveConnector::v_unthrottleEvent()
{}

prefix_ void senf::ppi::connector::PassiveConnector::notifyUnthrottle()
{
    if (throttled() && !nativeThrottled_) {
        Routes::const_iterator i (routes_.begin());
        Routes::const_iterator const i_end (routes_.end());
        for (; i != i_end; ++i)
            if ((*i)->throttled())
                break;
        if (i == i_end) {
            remoteThrottled_ = false;
            emitUnthrottle();
        }
    } 
    else
        remoteThrottled_ = false;
}

///////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::ActiveConnector

////////////////////////////////////////
// private members

prefix_ void senf::ppi::connector::ActiveConnector::v_init()
{
    if (! connected())
        notifyThrottle();
}

prefix_ void senf::ppi::connector::ActiveConnector::notifyThrottle()
{
    if (! throttled_) {
        throttled_ = true;
        if (throttleCallback_)
            throttleCallback_();
        NotifyRoutes::const_iterator i (notifyRoutes_.begin());
        NotifyRoutes::const_iterator const i_end (notifyRoutes_.end());
        for (; i != i_end; ++i)
            (*i)->notifyThrottle();
    }
}

prefix_ void senf::ppi::connector::ActiveConnector::notifyUnthrottle()
{
    if (throttled_) {
        throttled_ = false;
        if (unthrottleCallback_)
            unthrottleCallback_();
        NotifyRoutes::const_iterator i (notifyRoutes_.begin());
        NotifyRoutes::const_iterator const i_end (notifyRoutes_.end());
        for (; i != i_end; ++i)
            (*i)->notifyUnthrottle();
    }
}

prefix_ void senf::ppi::connector::ActiveConnector::registerRoute(ForwardingRoute & route)
{
    notifyRoutes_.push_back(&route);
}

///////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::InputConnector

prefix_ senf::Packet senf::ppi::connector::InputConnector::operator()()
{
    if (empty())
        v_requestEvent();
    Packet p;
    if (! empty()) {
        p = peek();
        queue_.pop_back();
        v_dequeueEvent();
    }
    return p;
}

////////////////////////////////////////
// private members

prefix_ void senf::ppi::connector::InputConnector::v_requestEvent()
{}

prefix_ void senf::ppi::connector::InputConnector::v_enqueueEvent()
{}

prefix_ void senf::ppi::connector::InputConnector::v_dequeueEvent()
{}

///////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::GenericActiveInput

////////////////////////////////////////
// private members

prefix_ void senf::ppi::connector::GenericActiveInput::v_requestEvent()
{
    request();
}

///////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::GenericPassiveInput

////////////////////////////////////////
// private members 

prefix_ void senf::ppi::connector::GenericPassiveInput::v_enqueueEvent()
{
    emit();
    qdisc_->update(*this, QueueingDiscipline::ENQUEUE);
}

prefix_ void senf::ppi::connector::GenericPassiveInput::v_dequeueEvent()
{
    qdisc_->update(*this, QueueingDiscipline::DEQUEUE);
}

prefix_ void senf::ppi::connector::GenericPassiveInput::v_unthrottleEvent()
{
    size_type n (queueSize());
    while (n) {
        emit();
        size_type nn (queueSize());
        if (n == nn)
            break;
        n = nn;
    }
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "Connectors.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
