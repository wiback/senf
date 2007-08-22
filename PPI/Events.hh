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
    \brief Events public header */

#ifndef HH_Events_
#define HH_Events_ 1

// Custom includes
#include <vector>
#include "Scheduler/ClockService.hh"
#include "predecl.hh"

//#include "Events.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace ppi {

    // Implementation: The concrete EventDescriptor implementation will need to set things up so
    // some callback (within the EventDescriptor implementation) will be called when the event
    // happens. This setup happens in 'v_enable()'. This internal handler sets up an EventType
    // instance if needed and calls 'callback()'. 
    //
    // 'callback()' will access the EventBinding wrapper to find the user-callback to signal. It
    // will do any needed internal processing, call that user callback and clean up afterwards.

    /** \brief Generic event interface baseclass

        The EventDescriptor baseclass provides an interface to manipulate events in a generic
        way. This allows to register events or to temporarily disable event processing.
     */ 
    class EventDescriptor
    {
    public:
        virtual ~EventDescriptor();

        bool enabled(); ///< Check, whether the event is currently enabled
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

        bool enabled_;

        typedef std::vector<ForwardingRoute*> Routes;
        Routes routes_;

        friend class ForwardingRoute;
    };
    
    template <class EventType, class Self>
    class EventImplementationHelper
    {
    protected:
        typedef typename detail::EventArgType<EventType>::type EventArg;

        void callback(EventArg event, ClockService::clock_type time);
        void callback(EventArg event);

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

    template <class EventType>
    class EventImplementation
        : public EventDescriptor, 
          public EventImplementationHelper< EventType, EventImplementation<EventType> >
    {
    public:
        typedef EventType Event;
        typedef typename detail::EventArgType<EventType>::type EventArg;

        module::Module & module() const;
        EventManager & manager() const;
        
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

///////////////////////////////hh.e////////////////////////////////////////
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
