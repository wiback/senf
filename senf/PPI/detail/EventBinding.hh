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
    \brief EventBinding public header */

#ifndef HH_SENF_PPI_detail_EventBinding_
#define HH_SENF_PPI_detail_EventBinding_ 1

// Custom includes
#include <senf/Scheduler/ClockService.hh>
#include <senf/PPI/predecl.hh>
#include "Callback.hh"

//#include "EventBinding.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {
namespace detail {

    /** \brief Internal: Association Event - Module - Handler, base-class */
    class EventBindingBase
    {
    public:
        ~EventBindingBase();

        EventManager & manager() const;
        module::Module & module() const;

    protected:
        EventBindingBase(EventManager & manager, module::Module & module,
                         EventDescriptor & descriptor);

        void eventTime(ClockService::clock_type time);

    private:
        EventManager * manager_;
        module::Module * module_;
        EventDescriptor * descriptor_;

        friend class senf::ppi::EventManager;
        friend class senf::ppi::EventDescriptor;
    };

    /** \brief Internal: Callback forwarders
     */
    template <class EventType, class Self>
    class EventBindingHelper
    {
    public:
        typedef typename detail::EventArgType<EventType>::type EventArg;

        void callback(EventArg event, ClockService::clock_type time);
        void callback(EventArg event);

    private:
        Self & self();
    };

#ifndef DOXYGEN

    template <class Self>
    class EventBindingHelper<void,Self>
    {
    public:
        void callback(ClockService::clock_type time);
        void callback();

    private:
        Self & self();
    };

#endif

    /** \brief Internal: Association Event - Module - Handler, event type specific */
    template <class EventType>
    class EventBinding
        : public EventBindingBase,
          public EventBindingHelper<EventType, EventBinding<EventType> >
    {
    public:
        typedef EventType Event;
        typedef typename detail::EventArgType<Event>::type EventArg;
        typedef typename detail::Callback<EventArg>::type Callback;

        EventBinding(EventManager & manager, module::Module & module, Callback callback,
                     EventDescriptor & descriptor);

    private:
        Callback callback_;

        friend class EventBindingHelper<EventType, EventBinding<EventType> >;
    };

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "EventBinding.cci"
//#include "EventBinding.ct"
#include "EventBinding.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u ../test"
// End:
