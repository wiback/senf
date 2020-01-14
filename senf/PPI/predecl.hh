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
