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
    \brief predecl public header */

#ifndef HH_SENF_PPI_predecl_
#define HH_SENF_PPI_predecl_ 1

// Custom includes

//#include "predecl.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    class Packet;

namespace ppi {

    class EventDescriptor;
    template <class EventType=void> class EventImplementation;
    class EventManager;
    class RouteBase;
    class ForwardingRoute;
    template <class Source, class Target> class Route;
    class QueueingDiscipline;
    class ModuleManager;

#ifndef DOXYGEN

    namespace detail {
        class EventBindingBase;
        template <class EvImpl> class EventBinding;
        template <class EventType> struct EventArgType;
        class NonForwardingRouteImplementation;
        class NonForwardingRouteToEventImplementation;
        class NonForwardingRouteFromEventImplementation;
        class ForwardForwardingRouteImplementation;
        class BackwardForwardingRouteImplementation;
        class ForwardForwardingRouteToEventImplementation;
        class BackwardForwardingRouteFromEventImplementation;
        template <class Source, class Target>
            class RouteImplementation;
        struct DisableStandardConnect;
    }

#endif

    namespace module {       
        class Module;       
        namespace detail {
            template <class Source, class Target> class RouteHelper;
        }
        class PassiveJoin;
        class PriorityJoin;
        class ActiveDuplicator;
    }

    namespace connector {
        class Connector;
        class ActiveConnector;
        class PassiveConnector;
        class InputConnector;
        class OutputConnector;
        class GenericActiveInput;
        class GenericActiveOutput;
        class GenericPassiveInput;
        class GenericPassiveOutput;
        template <class PacketType=Packet> class PassiveInput;
        template <class PacketType=Packet> class PassiveOutput;
        template <class PacketType=Packet> class ActiveInput;
        template <class PacketType=Packet> class ActiveOutput;

        class Jack;

#ifndef DOXYGEN

        namespace detail {
            template <class Self, class PacketType> class TypedInputMixin;
            template <class Self, class PacketType> class TypedOutputMixin;
        }

#endif

    }

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "predecl.cci"
//#include "predecl.ct"
//#include "predecl.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
