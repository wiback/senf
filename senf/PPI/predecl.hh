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
    \brief predecl public header */

#ifndef HH_SENF_PPI_predecl_
#define HH_SENF_PPI_predecl_ 1

// Custom includes

//#include "predecl.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    class Packet;

namespace ppi {

    class EventDescriptor;
    template <class EventType=void> class EventImplementation;
    class EventManager;
    class RouteBase;
    class ForwardingRoute;
    template <class Source, class Target> class Route;
    class ThrottlingDiscipline;
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
            template <class Source, class Target> struct RouteHelper;
        }
        template <class PacketType> class PassiveJoin;
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

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
