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
