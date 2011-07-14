// $Id$
//
// Copyright (C) 2010
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
//   Mathias Kretschmer <mtk@berlios.de>
//   Jens Moedeker <jmo@berlios.de>

/** \file
    \brief TimerEventProxy public header */

#ifndef HH_SENF_Scheduler_TimerEventProxy_
#define HH_SENF_Scheduler_TimerEventProxy_ 1

// Custom includes
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include <senf/Scheduler/ClockService.hh>
#include <senf/Scheduler/TimerEvent.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////
namespace senf {
namespace scheduler {

    /** \brief Deadline timer proxy

        The TimerEventProxy is meant to host long %term deadline timers to reduce the load of the
        Scheduler with a huge count of TimerEvent items. It registers deadline timer callbacks which
        will be called when the timer expires.

        The functionality is based on one TimerEvent instance per TimerEventProxy instance and could
        host a big count of timers.
     */
    template<typename IdType>
    class TimerEventProxy
    {
    public:
        typedef boost::function<void(ClockService::clock_type, IdType const &)> Callback;

        TimerEventProxy(std::string const & description = "");
                                        ///< Instantiate a TimerEventProxy
                                        /**< \param[in] description Descriptive name (purely informational) */

        void add(ClockService::clock_type timeout, IdType const & id, Callback cb);
                                        ///< Add new deadline timer

        bool remove(IdType const & id); ///< Remove timer by given \a id.

        std::vector<std::pair<ClockService::clock_type, IdType> > list() const;
                                        ///< Returns a vector of all active timers with timeout and id.

        ClockService::clock_type timeout(IdType const & id) const;
                                        ///< Returns timeout for given id
                                        /**< if no timer for this id is registered \a 0 is returned. */

        unsigned numEvents() const;  ///< Returns the number of pending timer events 

        void clear(); ///< Clears all pending timer events
        
    private:
#ifndef DOXYGEN
        struct Entry {
            ClockService::clock_type timeout;
            IdType id;
            Callback cb;

            Entry(ClockService::clock_type _timeout, IdType _id, Callback _cb)
                : timeout(_timeout), id(_id), cb(_cb) { }
        };
        struct Timeout {};
        struct Id {};
#endif
        // data structure to hold active timers
        typedef boost::multi_index_container<
            Entry,
            boost::multi_index::indexed_by<
                boost::multi_index::ordered_non_unique<
                    boost::multi_index::tag<Timeout>,
                    boost::multi_index::member<Entry, ClockService::clock_type, &Entry::timeout>
                >,
                boost::multi_index::ordered_unique<
                    boost::multi_index::tag<Id>,
                    boost::multi_index::member<Entry, IdType, &Entry::id>
                >
            >
        > EntrySet_t;
        typedef typename EntrySet_t::template index<Timeout>::type EntrySetByTimeout_t;
        typedef typename EntrySet_t::template index<Id>::type EntrySetById_t;

        EntrySet_t entrySet;
        EntrySetById_t & entrySetById;
        EntrySetByTimeout_t & entrySetByTimeout;

        scheduler::TimerEvent timer;

        void timerEvent();  // callback for the Scheduler timer event
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "TimerEventProxy.cci"
#include "TimerEventProxy.ct"
//#include "TimerEventProxy.cti"
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
