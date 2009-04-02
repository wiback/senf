// $Id$
//
// Copyright (C) 2008 
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
    \brief EventManager public header */

#ifndef HH_SENF_Scheduler_EventManager_
#define HH_SENF_Scheduler_EventManager_ 1

// Custom includes
#include <string>
#include <boost/iterator/filter_iterator.hpp>
#include "../boost/intrusive/ilist.hpp"
#include "../boost/intrusive/ilist_hook.hpp"
#include "../Utils/singleton.hh"
#include "../Utils/Console/LazyDirectory.hh"

//#include "EventManager.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace scheduler {
namespace detail {

    class Event;
    struct EventListTag;
    typedef boost::intrusive::ilist_base_hook<EventListTag> EventListBase;
    typedef boost::intrusive::ilist<EventListBase::value_traits<Event>, false> EventList;

    /** \brief
      */
    class Event
        : public EventListBase
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{
        
        explicit Event(std::string const & name);
        virtual ~Event();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        std::string const & name() const; ///< Get event name
        bool enabled() const;           ///< \c true, if event is enabled, \c false otherwise
        unsigned runCount() const;      ///< Number of times, event was fired
        char const * type() const;      ///< Event type code
        std::string info() const;       ///< Additional event information

    protected:
        void countRun();

    private:
        virtual bool v_enabled() const = 0;
        virtual char const * v_type() const = 0;
        virtual std::string v_info() const = 0;

        std::string name_;
        unsigned runCount_;
    };

    /** \brief
      */
    class EventManager
        : public singleton<EventManager>
    {
    public:
        using singleton<EventManager>::instance;
        using singleton<EventManager>::alive;
        
        struct IteratorFilter {
            bool operator()(Event const & e);
        };

        typedef boost::filter_iterator<
            IteratorFilter, EventList::const_iterator> iterator;

        void add(Event & event);
        void remove(Event & event);

        iterator begin() const;
        iterator end() const;

        void listEvents(std::ostream & os);

    protected:

    private:
        EventManager();

        EventList events_;

        friend class singleton<EventManager>;

#ifndef SENF_DISABLE_CONSOLE
        console::LazyDirectory consoleDir_;
#endif
    };

}}}

///////////////////////////////hh.e////////////////////////////////////////
#include "EventManager.cci"
//#include "EventManager.ct"
//#include "EventManager.cti"
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
