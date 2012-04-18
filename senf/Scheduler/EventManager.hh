// $Id$
//
// Copyright (C) 2008
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
    \brief EventManager public header */

#ifndef HH_SENF_Scheduler_EventManager_
#define HH_SENF_Scheduler_EventManager_ 1

// Custom includes
#include <string>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/list_hook.hpp>
#include <senf/Utils/singleton.hh>

//#include "EventManager.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace scheduler {
namespace detail {

    class Event;
    struct EventListTag;

    typedef boost::intrusive::list_base_hook< boost::intrusive::tag<EventListTag> > EventListBase;
    typedef boost::intrusive::list< Event,
                                    boost::intrusive::constant_time_size<false>,
                                    boost::intrusive::base_hook<EventListBase> > EventList;

    /** \brief
      */
    class Event
        : public EventListBase
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        explicit Event(std::string const & name);
        virtual ~Event();

        //\}
        //-////////////////////////////////////////////////////////////////////////

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
    };

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
