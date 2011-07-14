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
    \brief Events public header */

#ifndef HH_SENF_PPI_Events_
#define HH_SENF_PPI_Events_ 1

// Custom includes
#include <vector>
#include <senf/Scheduler/ClockService.hh>
#include "predecl.hh"

//#include "Events.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace ppi {

    namespace detail { class EventBindingBase; }

    /** \defgroup event_group Events

        Events provide notification of events outside the PPI framework: I/O activity, Timers
        etc. Events are very important since they drive the PPI: Without events, nothing will
        happen.

        \section event_impl Implementing Events

        All events are derived from EventImplementation which is based on EventDescriptor.
        \see EventImplementation \n
            \ref ppi_events
     */

    // Implementation: The concrete EventDescriptor implementation will need to set things up so
    // some callback (within the EventDescriptor implementation) will be called when the event
    // happens. This setup happens in 'v_enable()'. This internal handler sets up an EventType
    // instance if needed and calls 'callback()'.
    //
    // 'callback()' will access the EventBinding wrapper to find the user-callback to signal. It
    // will do any needed internal processing, call that user callback and clean up afterwards.

    /** \brief Generic event interface base-class

        The EventDescriptor base-class provides an interface to control events.

        \see \ref ppi_events
     */
    class EventDescriptor
    {
    public:
        virtual ~EventDescriptor();

        bool enabled() const; ///< Check, whether the event is currently enabled
        void enabled(bool v); ///< Enable or disable the event

    protected:
        EventDescriptor();

    private:
        virtual void v_enable() = 0;    ///< Called to enable the event delivery
        virtual void v_disable() = 0;   ///< Called to disable the event delivery

        virtual bool v_isRegistered() = 0;

        void notifyThrottle();
        void notifyUnthrottle();

        void registerRoute(ForwardingRoute & route);
        void unregisterRoute(ForwardingRoute & route);

        bool enabled_;
        bool throttled_;

        typedef std::vector<ForwardingRoute*> Routes;
        Routes routes_;

        detail::EventBindingBase * binding_;

        friend class ForwardingRoute;
        friend class detail::EventBindingBase;
    };

    /** \brief Internal: Callback forwarders
     */
    template <class EventType, class Self>
    class EventImplementationHelper
    {
    protected:
        typedef typename detail::EventArgType<EventType>::type EventArg;

        void callback(EventArg event, ClockService::clock_type time);
                                        ///< Forward event to user callback
                                        /**< \param[in] event Event argument to pass to the user
                                             callback
                                             \param[in] time Expected time of the event */
        void callback(EventArg event);  ///< Forward event to user callback
                                        /**< \param[in] event Event argument to pass to the user
                                             callback. */

    private:
        detail::EventBinding<EventType> & binding();
    };

#ifndef DOXYGEN

    template <class Self>
    class EventImplementationHelper<void,Self>
    {
    protected:
        void callback(ClockService::clock_type time);
        void callback();

    private:
        detail::EventBinding<void> & binding();
    };

#endif

    /** \brief Event implementation base class

        EventImplementation provides the base-class for all Event implementations.
        \code
        class SomeEvent : public EventImplementation<SomeEventArg>
        {
        public:
            SomeEvent() {}

        private:
            virtual void v_enable() {
                // register cb() to be called when the event occurs
            }

            virtual void v_disable() {
                // unregister cb()
            }

            void cb() {
                // Build event argument
                SomeEventArg arg (...);
                // Call the event callback
                callback(arg);
            }
        };
        \endcode

        Every event needs to implement v_enable() and v_disable(). v_enable() should register some
        member (in the example \c cb() ) to be called whenever the event occurs, while v_disable()
        should unregister it.

        The \a EventType argument to EventImplementation defines the type of argument passed to the
        user callback. It defaults to \c void. . This user callback is called from within the
        registered member (e.g. \c cb() ) by calling the inherited callback() member. This member
        takes an \a EventType reference as argument which will be forwarded to the user callback. If
        available, you should also provide the \e expected event time as a second argument.
     */
    template <class EventType>
    class EventImplementation
        : public EventDescriptor,
          public EventImplementationHelper< EventType, EventImplementation<EventType> >
    {
    public:
        typedef EventType Event;
        typedef typename detail::EventArgType<EventType>::type EventArg;

        module::Module & module() const; ///< Module in which the event is registered
        EventManager & manager() const; ///< EventManager of the event

    protected:
        EventImplementation();

    private:
        virtual bool v_isRegistered();
        void setBinding(detail::EventBinding<Event> & binding);

        detail::EventBinding<Event> * binding_;

        friend class EventManager;
        friend class EventImplementationHelper< EventType, EventImplementation<EventType> >;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "Events.cci"
//#include "Events.ct"
#include "Events.cti"
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
