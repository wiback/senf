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
