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
    \brief Module public header
*/

#ifndef HH_SENF_PPI_Module_
#define HH_SENF_PPI_Module_ 1

// Custom includes
#include <vector>
#include <boost/utility.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <senf/Scheduler/ClockService.hh>
#include "predecl.hh"
#include "ModuleManager.hh"

//#include "Module.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {

    namespace detail { class EventBindingBase; }

namespace module {

    /** \namespace senf::ppi::module
        \brief PPI Modules

        The modules build the PPI core. The PPI provides a set of general purpose infrastructure
        modules. For concrete applications, additional application specific processing modules need
        to be implemented.

        \section module_impl Implementing Modules

        All modules derive from senf::ppi::module::Module. See this class for a documentation on how
        to write new modules.

        \section infrastructure_modules General Purpose Modules

        The PPI provided general purpose modules can be grouped into several categories

        \li \ref io_modules receive external data or forward packets out of the PPI
        \li \ref routing_modules forward packets within the network
        \li \ref adapter_modules are used to connect incompatible connectors to each other
     */

    /** \defgroup io_modules Source/Sink Modules

        Source and Sink modules generate or absorb packets in some way: Reading data from a file
        descriptor, discarding packets etc.
     */

    /** \defgroup routing_modules Routing Modules

        Routing modules perform packet forwarding within the network. They do not process the packet
        data, they just route it.
     */

    /** \defgroup adapter_modules Adapter Modules

        Adapter modules adapt incompatible connectors to each other. They allow connection a pair of
        active or passive connectors.
     */

    /** \brief Module base-class

        senf::ppi::Module is the base-class of all PPI modules. It provides the module implementation
        with interfaces to several PPI facilities:

        \li Connector management
        \li Flow management (routing)
        \li Event handling

        To provide internal bookkeeping, most access to the PPI infrastructure is managed through
        this base class. This is an example module specification:
        \code
        class SomeModule : public senf::ppi::module::Module
        {
            SENF_PPI_MODULE(SomeModule);

            senf::FileHandle handle;

            // If needed, define events
            senf::ppi::IOEvent event;

        public:
            // Define connectors. Any number and type of connectors may be defined. Connectors
            // must be public since they need to be accessed to connect modules with each other.
            senf::ppi::connector::PassiveInput<> input;
            senf::ppi::connector::ActiveOutput<> output;

            SomeModule(senf::FileHandle h)
              : handle ( h ),
                event  ( handle, senf::ppi::IOEvent::Read )
            {
                // Set up routing. If some connector is not routed you need to explicitly state this
                // using noroute()
                route( input, output );
                route( event, output )
                    .autoThrottling( false );

                // Register event handlers
                registerEvent( event, &SomeModule::read );

                // Register passive connector handlers
                input.onRequest( &SomeModule::outputRequest );

                // If needed, you may register throttling event handlers
                output.onThrottle( &SomeModule::throttle );
                output.onUnthrottle( &SomeModule::unthrottle );
            }

            void read() {
                // Called whenever the 'handle' is readable. Read data, do processing and so
                // on. This example reads the data, puts it into an ethernet packet and sends the
                // packet out via the active output.
                output(senf::EthernetPacket::create(handle.read()))
            }

            void outputRequest() {
                // Called whenever a packet is sent into the input to. Here we just forward it to
                // the output if it is an EthernetPacket
                senf::EthernetPacket p (input().find<EthernetPacket>(senf::nothrow));
                if (p)
                    output(p);
            }

            void onThrottle() {
                // Called whenever a throttle notification comes in. Here, we just disable the
                // event (which is stupid since we should just not have disabled autoThrottling on
                // the route but this is only an example which tries to be simple ...)
                event.disable();
            }

            void onUnthrottle() {
                // and for unthrottle notifications
                event.enable();
            }

            void v_init() {
                // Optional. Called after before running the module but after connections have been
                // set up. This is either directly before senf::ppi::run() or senf::ppi::init() is
                // called or, for modules created while the PPI is already running, after returning
                // from all event handlers but before going back to the event loop.
            }

        };
        \endcode

        If your module only has a single input connector, you should call this connector \c
        input. If it has only a single output connector, you should call it \c output. This allows
        to setup connections without stating the connector explicitly (see senf::ppi::connect()).

        \see \ref ppi_modules
     */
    class Module
        : ModuleManager::Initializable, boost::noncopyable
    {
    public:
        virtual ~Module();

    protected:
        Module();

#ifdef DOXYGEN
        Route<connector::InputConnector, connector::OutputConnector> &
        route(connector::InputConnector & input, connector::OutputConnector & output);
                                        ///< Define flow information
                                        /**< Using the route() and noroute() members, the
                                             information flow within the module is defined. Routing
                                             may be defined between inputs, outputs and events. The
                                             routing information is used to perform automatic
                                             throttling. The throttling behavior may however be
                                             controlled manually.

                                             Even if no automatic throttling is desired <em>it is
                                             essential to define the flow information for all inputs
                                             and outputs</em>. Without flow information important
                                             internal state of the module cannot be
                                             initialized. This includes, explicitly defining
                                             terminal inputs and outputs using noroute. Event
                                             routing is optional however.

                                             The return value may be used to alter routing
                                             parameters like throttling parameters.

                                             \param[in] input Data source, object which controls
                                                 incoming data (connector or event)
                                             \param[in] output Data target, object which controls
                                                 outgoing data (connector or event)
                                             \returns Route instance describing this route
                                             \see \ref ppi_throttling */

        Route<connector::InputConnector, EventDescriptor> &
        route(connector::InputConnector & input, EventDescriptor & output);
                                        ///< Define flow information
                                        /**< Route from a connector to an event.  Routing from a
                                             connector to an event defines the event as the
                                             conceptual 'receiver' of the data. This means, the
                                             event is controlling the processing of received data
                                             packets (Example: Routing from an input to an IOEvent
                                             defines, that input data will be processed whenever the
                                             event is signaled.).

                                             This event routing allows to automatically
                                             enable/disable the event on throttling notifications.

                                             \see \ref route() */

        Route<EventDescriptor, connector::OutputConnector> &
        route(EventDescriptor & input, connector::OutputConnector & output);
                                        ///< Define flow information
                                        /**< Route from an event to a connector. Routing from an
                                             event to a connector defines the event as the
                                             conceptual 'source' of the data. This means, the event
                                             controls how packets are sent (Example: Routing from an
                                             IOEvent to an output defines, that output data will be
                                             generated whenever the event is signaled).

                                             This event routing allows to automatically
                                             enable/disable the event on throttling notifications.

                                             \see \ref route() */
#else
#       define route_decl(Source, Target)               \
            Route<Source, Target> &                     \
            route(Source & source, Target & target);

        route_decl( connector::GenericPassiveInput, connector::GenericActiveOutput);
        route_decl( connector::GenericPassiveInput, connector::GenericPassiveOutput);
        route_decl( connector::GenericActiveInput,  connector::GenericPassiveOutput);
        route_decl( connector::GenericActiveInput,  connector::GenericActiveOutput);
        route_decl( connector::GenericPassiveInput, EventDescriptor);
        route_decl( connector::GenericActiveInput,  EventDescriptor);
        route_decl( EventDescriptor, connector::GenericPassiveOutput);
        route_decl( EventDescriptor, connector::GenericActiveOutput);

#undef Route_decl
#endif

        void noroute(connector::Connector & connector); ///< Define terminal connectors
                                        /**< The noroute() member explicitly declares, that a
                                             connector is terminal and does not directly
                                             receive/forward data from/to some other
                                             connector. <em>It is mandatory to define routing
                                             information for terminal connectors</em>.

                                             See the route() documentation for more on routing

                                             \param[in] connector Terminal connector to declare */

#ifndef DOXYGEN
        template <class Descriptor, class Target>
        void registerEvent(Descriptor & descriptor, Target target);
#else
        template <class Target>
        void registerEvent(EventDescriptor & descriptor, Target target);
                                        ///< Register an external event
                                        /**< The \a target argument may be either an arbitrary
                                             callable object or it may be a member function pointer
                                             pointing to a member function of the Module derived
                                             classed. The handler may \e optionally take an Argument
                                             of type <tt>Descriptor::Event const &</tt>. This object
                                             allows to access detailed information on the event
                                             delivered.

                                             The \a descriptor describes the event to signal like a
                                             timer event or some type of I/O event on a file
                                             descriptor or socket.

                                             \param[in] target The handler to call whenever the
                                                 event is signaled
                                             \param[in] descriptor The type of event to register
                                             \note The real implementation has the second arguments
                                                 type as an additional template parameter. */
#endif

        ClockService::clock_type time() const; ///< Time-stamp of the currently processing event
                                        /**< If available, this returns the scheduled time of the
                                             event. */

        ClockService::clock_type now() const; ///< Current time of the currently processing event

#ifndef DOXYGEN
        virtual void macro_SENF_PPI_MODULE_missing() = 0;

    private:
#endif
        virtual void v_init();          ///< Called after module setup
                                        /**< This member is called directly before the PPI (resumes)
                                             execution. It is called after connections have been
                                             setup before entering the PPI main loop.

                                             You may overload this member. Your overload should
                                             always call the base-class implementation. */

#ifndef DOXYGEN
    public:
#endif
        void destroy();

    private:
        EventManager & eventManager() const;
        ModuleManager & moduleManager() const;

        void registerConnector(connector::Connector & connector);
        void unregisterConnector(connector::Connector & connector);
        void unregisterEvent(EventDescriptor & event);

        RouteBase & addRoute(std::auto_ptr<RouteBase> route);

        typedef std::vector<connector::Connector *> ConnectorRegistry;
        ConnectorRegistry connectorRegistry_;

        typedef boost::ptr_vector<RouteBase> RouteInfoBase;
        RouteInfoBase routes_;

        template <class Source, class Target>
        friend class detail::RouteHelper;
        friend class senf::ppi::ModuleManager;
        friend class connector::Connector;
        friend class senf::ppi::detail::EventBindingBase;
    };

    /** \brief Define PPI Module

        Every module must begin by using this macro.

        \see senf::ppi::module::Module
     */
#   define SENF_PPI_MODULE(name)                                                                  \
    public:                                                                                       \
        ~ name() { destroy(); }                                                                   \
        void macro_SENF_PPI_MODULE_missing() {}                                                   \
    private:

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "Module.cci"
#include "Module.ct"
//#include "Module.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
