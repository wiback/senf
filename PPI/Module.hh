// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief Module public header 
*/

#ifndef HH_Module_
#define HH_Module_ 1

// Custom includes
#include <vector>
#include <boost/utility.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include "../Scheduler/ClockService.hh"
#include "predecl.hh"

//#include "Module.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {
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
        \li \ref sourcesink_modules generate or absorb packets internally
        \li \ref routing_modules forward packets within the network
        \li \ref adapter_modules are used to connect incompatible connectors to each other

        \todo Implement Spliters: PassiveSplitter, PrioritySplitter, CloneSplitter
     */

    /** \defgroup io_modules Input/Output Modules

        Input/Output Modules receive data from external sources or forward data from the PPI to
        outside targets.
     */

    /** \defgroup sourcesink_modules Source/Sink Modules

        Source and Sink modules generate or absorb packets internally. In contrast to \ref
        io_modules, they do not communicate outside the PPI.
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
            senf::ppi::connector::PassiveInput input;
            senf::ppi::connector::ActiveOutput output;

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
                registerEvent( &SomeModule::read, event );

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

        };
        \endcode

        If your module only has a single input connector, you should call this connector \c
        input. If it has only a single output connector, you should call it \c output. This allows
        to setup connections without stating the connector explicitly (see senf::ppi::connect()).
     */
    class Module
        : boost::noncopyable
    {
    public:
        virtual ~Module();

    protected:
        Module();

        template <class Source, class Target>
        Route<Source, Target> & route(Source & source, Target & target); 
                                        ///< Define flow information
                                        /**< Using the route() and noroute() members, the
                                             information flow within the module is defined. Routing
                                             may be specified either between inputs, outputs and
                                             events. The routing information is used to perform
                                             automatic throttling. The throttling behavior may
                                             however be controlled manually.

                                             Even if no automatic throttling is desired <em>it is
                                             vital to define the flow information for all inputs and
                                             outputs</em>. Without flow information important
                                             internal state of the module cannot be
                                             initialized. This includes, explicitly defining
                                             terminal inputs and outputs using noroute. Event
                                             routing however is optional.

                                             The return value may be used to alter routing
                                             parameters like throttling parameters.
                                             
                                             \param[in] source Data source, object which controls
                                                 incoming data
                                             \param[in] target Data target, object which controls
                                                 outgoing data
                                             \returns Route instance describing this route */

        void noroute(connector::Connector & connector); ///< Define terminal connectors
                                        /**< The noroute() member explicitly declares, that a
                                             connector is terminal and does not directly
                                             receive/forward data from/to some other
                                             connector. <em>It is mandatory to define routing
                                             information for terminal connectors</em>.

                                             See the route() documentation for more on routing
                                             
                                             \param[in] connector Terminal connector to declare */

        template <class Target, class Descriptor>
        void registerEvent(Target target, Descriptor & descriptor);
                                        ///< Register an external event
                                        /**< The \a target argument may be either an arbitrary
                                             callable object or it may be a member function pointer
                                             pointing to a member function of the Module derived
                                             classed. The handler may \e optionally take an Argument
                                             of type <tt>Descriptor::Event const &</tt>. This object
                                             allows to access detailed information on the event
                                             delivered.

                                             The \a descriptor describes the event to signal. This

                                             may be a timer event or some type of I/O event on a
                                             file descriptor or socket.

                                             \param[in] target The handler to call whenever the
                                                 event is signaled
                                             \param[in] descriptor The type of event to register */

        ClockService::clock_type time() const; ///< Time-stamp of the currently processing event
                                        /**< If available, this returns the scheduled time of the
                                             event. */

        ClockService::clock_type now() const; ///< Current time of the currently processing event

#ifndef DOXYGEN
        virtual void macro_SENF_PPI_MODULE_missing() = 0;
#endif

#ifndef DOXYGEN
    private:
#endif
        virtual void init();            ///< Called just before the network is run

#ifndef DOXYGEN
    public:
#endif
        void destroy();
        
    private:
        EventManager & eventManager() const;
        ModuleManager & moduleManager() const;
        
        void registerConnector(connector::Connector & connector);
        RouteBase & addRoute(std::auto_ptr<RouteBase> route);

        typedef std::vector<connector::Connector *> ConnectorRegistry;
        ConnectorRegistry connectorRegistry_;

        typedef boost::ptr_vector<RouteBase> RouteInfoBase;
        RouteInfoBase routes_;

        template <class Source, class Target>
        friend class detail::RouteHelper;
        friend class senf::ppi::ModuleManager;
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

///////////////////////////////hh.e////////////////////////////////////////
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
