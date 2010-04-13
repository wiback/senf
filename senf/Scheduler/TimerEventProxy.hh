// $Id$
//
// Copyright (C) 2010
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Mathias Kretschmer <mtk@berlios.de>
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

#ifdef SENF_DEBUG
#   define BOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING
#   define BOOST_MULTI_INDEX_ENABLE_SAFE_MODE
#endif

#include <boost/range/iterator_range.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/key_extractors.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>

#include <senf/Scheduler/ClockService.hh>
#include <senf/Scheduler/TimerEvent.hh>
#include <senf/Utils/Console/Console.hh>

namespace senf {
namespace scheduler {


    /** \brief Deadline timer proxy

        The TimerEventProxy is meant to host long term deadline timers to reduce the load of the
        Scheduler with a hugh count of TimerEvent items. It registers deadline timer callbacks which
        will be called when the timer expires.

        The functionality is based on one TimerEvent instance per TimerEventProxy instance and could
        host a big count of timers.
     */
    template<class T>
    class TimerEventProxy {
    private:

        template<class T_>
        struct Entry {
        public:
            senf::ClockService::clock_type timeout;
            T id;
            boost::function<void(senf::ClockService::clock_type, T_ const &)> fkt;

            bool operator<(const Entry<T_> & e) const {
                return id < e.id;
            }
            //  bool operator==(const Entry<T> &e)const{return id == e.id;}
            Entry(senf::ClockService::clock_type _timeout, T_ _id, boost::function<void(
                    senf::ClockService::clock_type, T_)> _fkt) :
                timeout(_timeout), id(_id), fkt(_fkt) {
            }
        };


        senf::scheduler::TimerEvent timer;

        //
        // data structure to hold active timers
        //
        struct Timeout {};
        struct Id {};
        typedef boost::multi_index_container<Entry<T> ,
                boost::multi_index::indexed_by<
                        boost::multi_index::ordered_non_unique<
                                boost::multi_index::tag<Timeout>,
                                boost::multi_index::member<Entry<T> ,
                                        senf::ClockService::clock_type,
                                        &Entry<T>::timeout> >,
                        boost::multi_index::ordered_unique<boost::multi_index::tag<
                                Id>, boost::multi_index::identity<Entry<T> > > > >
                EntrySet;

        typedef typename EntrySet::template index<Timeout>::type
                EntrySetByTimeout_t;
        typedef typename EntrySet::template index<Id>::type EntrySetById_t;

        EntrySet entrySet;
        EntrySetById_t & entrySetById;
        EntrySetByTimeout_t & entrySetByTimeout;

    private:
        // callback for the Scheduler timer event
        void timerEvent();

    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types
        typedef boost::function<void(senf::ClockService::clock_type, T const &)> Callback;

        TimerEventProxy();
        ///< Instantiate a TimerEventProxy

        TimerEventProxy(std::string const & name, senf::console::DirectoryNode & node);
        ///< Instantiate a TimerEventProxy and add the list command to the give DirectoryNode

        void add(senf::ClockService::clock_type timeout, T const &id, Callback cb);
        ///< Add new deadline timer
        bool del(T const & id);
        ///< Remove timer by given \a id.
        std::vector<std::pair<senf::ClockService::clock_type, T> > list();
        ///< Returns a vector of all active timers with timeout and id.
    };
}
}

#include "TimerEventProxy.ct"

#endif
