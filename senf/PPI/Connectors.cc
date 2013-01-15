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
    \brief Connectors non-inline non-template implementation */

#include "Connectors.hh"
#include "Connectors.ih"

// Custom includes
#include <senf/Utils/Console/ParsedCommand.hh>
#include "ModuleManager.hh"
#include "Module.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::Connector

prefix_ senf::ppi::connector::Connector::Connector()
    : peer_(NULL), module_(NULL), consoleDir_(this), traceState_(NO_TRACING)
{
    namespace fty = console::factory;
    consoleDir().add("connected", fty::Command( &Connector::connected, this));
#ifndef SENF_PPI_NOTRACE
    consoleDir().add("tracing", fty::Command(
            SENF_MEMFNP(void, Connector, tracingState, (TraceState)), this));
    consoleDir().add("tracing", fty::Command(
            SENF_MEMFNP(TraceState, Connector, tracingState, () const), this));
#endif
}

prefix_ senf::ppi::connector::Connector::~Connector()
{
    if (connected()) {
        Connector & peer (*peer_);
        peer_->peer_ = 0;
        if (! peer.initializationScheduled())
            peer.enqueueInitializable();
        peer.v_disconnected();
    }
}

prefix_ void senf::ppi::connector::Connector::connect(Connector & target)
{
    // The connector is not registered -> route() or noroute() statement missing
    SENF_ASSERT( module_,
                 "senf::ppi::connector::Connector::connect(): (source) "
                 "Missing route() or noroute()" );
    // The connector is already connected
    if (peer_)
        throw DuplicateConnectionException()
            << " in module: " << prettyName(typeid(*module_))
            << ", source module: " << prettyName(typeid(peer_->module()));
    // The target connector is not registered -> route() or noroute() statement missing
    SENF_ASSERT( target.module_,
                 "senf::ppi::connector::Connector::connect(): (target) "
                 "Missing route() or noroute()" );
    // The target connector is already connected
    if (target.peer_)
        throw DuplicateConnectionException()
            << " in module: " << prettyName(typeid(*module_))
            << ", target module: " << prettyName(typeid(target.peer_->module()));
    if (! (v_packetTypeId() == typeid(void) ||
           target.v_packetTypeId() == typeid(void) ||
           v_packetTypeId() == target.v_packetTypeId()) )
        throw IncompatibleConnectorsException()
            << ": " << prettyName(v_packetTypeId())
            << " [in module " << prettyName(typeid(*module_))  << "] "
            << ", " << prettyName(target.v_packetTypeId())
            << " [in module " << prettyName(typeid(*target.module_)) << "]";

    dynamic_cast<InputConnector &>(target).v_checkedPacketCast(
            v_packetTypeId() != target.v_packetTypeId());

    peer_ = &target;
    target.peer_ = this;

    if (! initializationScheduled())
        enqueueInitializable();
    if (! peer().initializationScheduled())
        peer().enqueueInitializable();

    v_connected();
    peer_->v_connected();

}

senf::ppi::connector::Connector::TraceState senf::ppi::connector::Connector::staticTraceState_ (
    senf::ppi::connector::Connector::NO_TRACING);

prefix_ void senf::ppi::connector::Connector::trace(Packet const & p, char const * label)
{
    if (tracingState() == NO_TRACING)
        return;
    SENF_LOG_BLOCK(({
        std::string type (prettyName(p.typeId().id()));
        log << "PPI packet trace: " << label << " 0x" << std::hex << p.id() << " "
            << type.substr(21, type.size()-22) << " on " << & module() << " "
            << prettyName(typeid(module())) << " connector 0x" << this << "\n";
        if (tracingState() == TRACE_CONTENTS)
            p.dump(log);
    }));
}

prefix_ void senf::ppi::connector::Connector::throttleTrace(char const * label,
                                                            char const * type)
{
    if (tracingState() == NO_TRACING)
        return;
    SENF_LOG_BLOCK(({
        log << "PPI throttling trace: " << label << " " << type << " on " << & module()
            << " " << prettyName(typeid(module())) << " connector 0x" << this << "\n";
    }));
}

namespace senf { namespace ppi { namespace connector {

    SENF_CONSOLE_REGISTER_ENUM_MEMBER(
        Connector, TraceState, (NO_TRACING)(TRACE_IDS)(TRACE_CONTENTS) );

}}}

namespace {

    struct ConsoleRegister
    {
        ConsoleRegister();
    };

    ConsoleRegister::ConsoleRegister()
    {
#ifndef SENF_PPI_NOTRACE
        senf::ppi::ModuleManager::instance().consoleDir()
            .add("tracing", senf::console::factory::Command(
                     SENF_FNP(void, senf::ppi::connector::Connector::staticTracingState,
                              (senf::ppi::connector::Connector::TraceState)))
                .arg("state", "new tracing state")
                .doc("Log every packet sent or received by any module.\n"
                     "There are three different tracing levels:\n"
                     "\n"
                     "    NO_TRACING      don't output any tracing information\n"
                     "    TRACE_IDS       trace packet id's but do not show packet contents\n"
                     "    TRACE_CONTENTS  trace complete packet contents\n"
                     "\n"
                     "A log message is generated whenever the packet traverses a connector. The\n"
                     "TRACE_IDS log message has the following format:\n"
                     "\n"
                     "    PPI packet trace: <direction> <packet-id> <packet-type>\n"
                     "                      on <module-id> <module-type> connector <connector-id>\n"
                     "    PPI throttling trace: <direction> <throttle-msg>\n"
                     "                      on <module-id> <module-type> connector <connector-id>\n"
                     "\n"
                     "The fields are:\n"
                     "\n"
                     "    direction       'IN' for packets/throttle notifications entering the module,\n"
                     "                    'OUT' for packets/throttle notifications leaving it\n"
                     "    packet-id       Numeric unique packet id. This value is unique for packets\n"
                     "                    alive at the same time, packets at different times may (and\n"
                     "                    will) share id's\n"
                     "    packet-type     The type of the packet header\n"
                     "    module-id       Unique module id\n"
                     "    module-type     Type of the module the packet is sent to/from\n"
                     "    connector-id    Unique connector id\n"
                     "    throttle-msg    Type of throttling event\n")
                );
#endif
    }

    ConsoleRegister consoleRegister;

}

prefix_ void senf::ppi::connector::Connector::disconnect()
{
    // Cannot disconnected a non-connected connector
    SENF_ASSERT( peer_,
                 "senf::ppi::connector::Connector::disconnect(): Not connected" );

    Connector & peer (*peer_);
    peer_ = 0;
    peer.peer_ = 0;

    if (! initializationScheduled())
        enqueueInitializable();
    if (! peer.initializationScheduled())
        peer.enqueueInitializable();

    v_disconnected();
    peer.v_disconnected();
}

prefix_ std::type_info const & senf::ppi::connector::Connector::v_packetTypeId()
{
    return typeid(void);
}

prefix_ void senf::ppi::connector::Connector::unregisterConnector()
{
    if (module_)
        module_->unregisterConnector(*this);
}

prefix_ void senf::ppi::connector::Connector::setModule(module::Module & module)
{
    module_ = &module;
    consoleDir().remove( "module", senf::nothrow);
    consoleDir().add( "module", console::factory::Link(module.sysConsoleDir()));
}

prefix_ senf::ppi::module::Module & senf::ppi::connector::Connector::module()
    const
{
    // The connector is not registered in the module -> probably a route() or noroute() statement is
    // missing.
    SENF_ASSERT(module_, "Connector not registered: Missing route() or noroute()");
    return *module_;
}

prefix_ void senf::ppi::connector::Connector::v_disconnected()
{
    consoleDir().remove( "peer", senf::nothrow);
}

prefix_ void senf::ppi::connector::Connector::v_connected()
{
    if (! consoleDir().hasChild("peer"))
        consoleDir().add( "peer", console::factory::Link(peer_->consoleDir()));
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::PassiveConnector

prefix_ senf::ppi::connector::PassiveConnector::PassiveConnector()
    : peer_(NULL), remoteThrottled_(false), nativeThrottled_(false)
{
    namespace fty = console::factory;
    consoleDir().add("throttled", fty::Command( &PassiveConnector::throttled, this));
}

prefix_ senf::ppi::connector::PassiveConnector::~PassiveConnector()
{
    // Must be here and NOT in base so it is called before destructing the routes_ member
    unregisterConnector();
}

prefix_ void senf::ppi::connector::PassiveConnector::v_disconnected()
{
    Connector::v_disconnected();
    peer_ = 0;
}

prefix_ void senf::ppi::connector::PassiveConnector::v_connected()
{
    Connector::v_connected();
    peer_ = & dynamic_cast<ActiveConnector&>(Connector::peer());
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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

prefix_ void senf::ppi::connector::PassiveConnector::registerRoute(ForwardingRoute & route)
{
    routes_.push_back(&route);
}

prefix_ void senf::ppi::connector::PassiveConnector::unregisterRoute(ForwardingRoute & route)
{
    Routes::iterator i (std::find(routes_.begin(), routes_.end(), &route));
    if (i != routes_.end())
        routes_.erase(i);
}

prefix_ void senf::ppi::connector::PassiveConnector::v_unthrottleEvent()
{}

prefix_ void senf::ppi::connector::PassiveConnector::notifyUnthrottle()
{
    if (std::find_if(routes_.begin(), routes_.end(),
                     boost::bind(&ForwardingRoute::throttled, _1)) == routes_.end()) {
        remoteThrottled_ = false;
        if (!nativeThrottled_)
            emitUnthrottle();
    } else {
        SENF_PPI_THROTTLE_TRACE("OUT", "not forwarding unthrottle event");
    }
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::ActiveConnector

prefix_ senf::ppi::connector::ActiveConnector::~ActiveConnector()
{
    // Must be here and NOT in base so it is called before destructing the routes_ member
    unregisterConnector();
}

prefix_ void senf::ppi::connector::ActiveConnector::v_disconnected()
{
    Connector::v_disconnected();
    peer_ = 0;
}

prefix_ void senf::ppi::connector::ActiveConnector::v_connected()
{
    Connector::v_connected();
    peer_ = & dynamic_cast<PassiveConnector&>(Connector::peer());
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// private members

prefix_ void senf::ppi::connector::ActiveConnector::v_init()
{
    if (! connected())
        notifyThrottle();
}

prefix_ void senf::ppi::connector::ActiveConnector::notifyThrottle()
{
    SENF_PPI_THROTTLE_TRACE("IN ", "throttle");
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
    SENF_PPI_THROTTLE_TRACE("IN ", "unthrottle");
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

prefix_ void senf::ppi::connector::ActiveConnector::unregisterRoute(ForwardingRoute & route)
{
    NotifyRoutes::iterator i (std::find(notifyRoutes_.begin(), notifyRoutes_.end(), &route));
    if (i != notifyRoutes_.end())
        notifyRoutes_.erase(i);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::InputConnector

prefix_ senf::Packet const & senf::ppi::connector::InputConnector::operator()()
{
    static Packet nullPacket;

    if (empty())
        v_requestEvent();
    if (fastPacket_) {
        Packet const * p = fastPacket_;
        fastPacket_ = NULL;
        v_dequeueEvent();
        SENF_PPI_TRACE(*p, "IN ");
        return *p;
    }
    if (! queue_.empty()) {
        slowPacket_ = queue_.back();
        queue_.pop_back();
        v_dequeueEvent();
        SENF_PPI_TRACE(slowPacket_, "IN ");
        return slowPacket_;
    } else {
        SENF_PPI_TRACE(nullPacket, "IN ");
        return nullPacket;
    }
}

prefix_ void senf::ppi::connector::InputConnector::v_disconnected()
{
    Connector::v_disconnected();
    peer_ = 0;
}

prefix_ void senf::ppi::connector::InputConnector::v_connected()
{
    Connector::v_connected();
    peer_ = & dynamic_cast<OutputConnector&>(Connector::peer());
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// private members

prefix_ void senf::ppi::connector::InputConnector::v_requestEvent()
{}

prefix_ void senf::ppi::connector::InputConnector::v_enqueueEvent()
{}

prefix_ void senf::ppi::connector::InputConnector::v_dequeueEvent()
{}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::OutputConnector

prefix_ void senf::ppi::connector::OutputConnector::v_disconnected()
{
    Connector::v_disconnected();
    peer_ = 0;
}

prefix_ void senf::ppi::connector::OutputConnector::v_connected()
{
    Connector::v_connected();
    peer_ = & dynamic_cast<InputConnector&>(Connector::peer());
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::GenericActiveInput

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// private members

prefix_ void senf::ppi::connector::GenericActiveInput::v_requestEvent()
{
    request();
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::GenericPassiveInput

prefix_ void senf::ppi::connector::GenericPassiveInput::v_disconnected()
{
    PassiveConnector::v_disconnected();
    InputConnector::v_disconnected();
    peer_ = 0;
}

prefix_ void senf::ppi::connector::GenericPassiveInput::v_connected()
{
    PassiveConnector::v_connected();
    InputConnector::v_connected();
    peer_ = & dynamic_cast<GenericActiveOutput&>(Connector::peer());
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// private members

prefix_ void senf::ppi::connector::GenericPassiveInput::v_enqueueEvent()
{
    emit();
    if (qdisc_)
        qdisc_->update(*this, QueueingDiscipline::ENQUEUE);
}

prefix_ void senf::ppi::connector::GenericPassiveInput::v_dequeueEvent()
{
    if (qdisc_)
        qdisc_->update(*this, QueueingDiscipline::DEQUEUE);
}

prefix_ void senf::ppi::connector::GenericPassiveInput::qdisc(QueueingDiscipline::None_t)
{
    qdisc_.reset( 0);
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::GenericPassiveOutput

prefix_ void senf::ppi::connector::GenericPassiveOutput::v_disconnected()
{
    PassiveConnector::v_disconnected();
    OutputConnector::v_disconnected();
    peer_ = 0;
}

prefix_ void senf::ppi::connector::GenericPassiveOutput::v_connected()
{
    PassiveConnector::v_connected();
    OutputConnector::v_connected();
    peer_ = & dynamic_cast<GenericActiveInput&>(Connector::peer());
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::GenericActiveInput

prefix_ void senf::ppi::connector::GenericActiveInput::v_disconnected()
{
    ActiveConnector::v_disconnected();
    InputConnector::v_disconnected();
    peer_ = 0;
}

prefix_ void senf::ppi::connector::GenericActiveInput::v_connected()
{
    ActiveConnector::v_connected();
    InputConnector::v_connected();
    peer_ = & dynamic_cast<GenericPassiveOutput&>(Connector::peer());
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::ppi::connector::GenericActiveOutput

prefix_ void senf::ppi::connector::GenericActiveOutput::v_disconnected()
{
    ActiveConnector::v_disconnected();
    OutputConnector::v_disconnected();
    peer_ = 0;
}

prefix_ void senf::ppi::connector::GenericActiveOutput::v_connected()
{
    ActiveConnector::v_connected();
    OutputConnector::v_connected();
    peer_ = & dynamic_cast<GenericPassiveInput&>(Connector::peer());
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
