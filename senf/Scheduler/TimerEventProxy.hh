// $Id$
//
// Copyright (C) 2010
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Mathias Kretschmer <mtk@berlios.de>
//     Jens Moedeker <jens.moedeker@fokus.fraunhofer.de>
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
