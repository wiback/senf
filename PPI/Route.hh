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
    \brief Route public header */

#ifndef HH_Route_
#define HH_Route_ 1

// Custom includes
#include <boost/type_traits.hpp>
#include "predecl.hh"

//#include "Route.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {

    /** \brief Routing base class

        Routing information is defined within each module to define the control flow. RouteBase is
        the generic base class for all routing entries.
     */
    class RouteBase
    {
    public:
        virtual ~RouteBase();

#ifdef DOXYGEN
        Source & source() const;        ///< Routing source
                                        /**< \note The real implementation is in the \c
                                             BaseRouteImplementation template in \c Route.ih. This
                                             class is internal and not documented. */
        Target & target() const;        ///< Routing target
                                        /**< \note The real implementation is in the \c
                                             BaseRouteImplementation template in \c Route.ih. This
                                             class is internal and not documented. */
#endif

    protected:
        RouteBase(module::Module & module);

    private:
        module::Module * module_;
    };

    /** \brief Forwarding route base class

        All routes which may forward control information are based on
        ForwardingRoute. ForwardingRoute provides methods to control and query the throttling
        behavior.
     */
    class ForwardingRoute
        : public RouteBase
    {
    public:
        bool autoThrottling() const;    ///< Query current autoThrottling state
        void autoThrottling(bool state); ///< Change automatic throttle notification forwarding
                                        /**< By default, throttle notifications are automatically
                                             forwarded from active to passive connectors. This may
                                             be disabled by setting the authoThrottling state to \c
                                             false.
                                             
                                             Routing from/to an event to/from a passive connector
                                             will automatically create throttling notifications on
                                             the connector whenever the event is disabled. Routing
                                             form/to an event to/from an active connector will
                                             disable the event whenever a throttling notification
                                             comes in. Respective for unthrottle notifications.

                                             \param[in] state New throttle forwarding state */

        bool throttled() const;         ///< \c true, if the route is throttled
                                        /**< This member checks only the automatic throttling
                                             state. If autoThrottling() is \c false, this member
                                             will always return \c false. */
        
    protected:
        ForwardingRoute(module::Module & module);

        // Called to register this route with the connectors forwarding information base
        template <class T> void registerRoute(T & ob);

        template <class T> void notifyThrottle(T & ob);
        template <class T> void notifyUnthrottle(T & ob);

    private:
        // called to forward a throttling notification along the route
        void notifyThrottle();
        void notifyUnthrottle();

        // Implemented in the derived classes to forward throttling notifications
        virtual void v_notifyThrottle() = 0;
        virtual void v_notifyUnthrottle() = 0;
        virtual bool v_throttled() const = 0;

        bool autoThrottling_;

        friend class connector::ActiveConnector;
    };

}}

// We need detail::RouteImplementation here ...
#include "Route.ih"

namespace senf {
namespace ppi {

    /** \brief Route descriptor
        
        Route instances are created by Module::route statements. The Route class provides an
        interface to manipulate the flow processing.

        Depending on the type of route, one of the following classes will be a baseclass:

        <dl> <dt>ForwardingRoute</dt><dd>If the route is a \e forwarding route. This is a route
        which forwards throttling notifications. This is the case, if one of the route endpoints is
        a notify source (a connector::ActiveConnector) and the other is a
        notify target (a connector::PassiveConnector or an EventDescriptor).

        <dt>RouteBase</dt><dd>If the route is not a forwarding route, it is based directly on the
        generic route base class</dd></dl>
     */
    template <class Source, class Target>
    class Route
        : public detail::RouteImplementation<Source,Target>
    {
    private:
        typedef detail::RouteImplementation<Source,Target> Base;
        typedef detail::RouteImplementation<Source,Target> Implementation;
        
        Route(module::Module & module, Source & source, Target & target);

        friend class module::Module;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "Route.cci"
#include "Route.ct"
#include "Route.cti"
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
