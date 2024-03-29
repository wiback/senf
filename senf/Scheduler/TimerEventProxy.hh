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
    \brief TimerEventProxy public header */

#ifndef HH_SENF_Scheduler_TimerEventProxy_
#define HH_SENF_Scheduler_TimerEventProxy_ 1

// Custom includes
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <senf/Scheduler/ClockService.hh>
#include <senf/Scheduler/TimerEvent.hh>
#include <senf/Utils/StatisticAccumulator.hh>

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
        typedef boost::function<void(ClockService::clock_type const &, IdType const &)> Callback;
        typedef boost::function<void(IdType const & id, senf::ClockService::clock_type const & duration, StatisticsData const & durationsStats)> DurationExceededCallback;

        TimerEventProxy(std::string const & description = "",
                        senf::ClockService::clock_type const & callbackDurationWarningThreshold = senf::ClockService::clock_type(0),
                        DurationExceededCallback _decb = 0);
                                        ///< Instantiate a TimerEventProxy
                                        /**< \param[in] description Descriptive name (purely informational) */
                                        /**< \param[in] If !=0, the max. time a callback may take for execuion before a warning is generated */

        void add(ClockService::clock_type const & timeout, IdType const & id, Callback cb);
                                        ///< Add new deadline timer

        bool remove(IdType const & id); ///< Remove timer by given \a id.

        std::vector<std::pair<ClockService::clock_type, IdType> > list() const;
                                        ///< Returns a vector of all active timers with timeout and id.

        ClockService::clock_type timeout(IdType const & id) const;
                                        ///< Returns timeout for given id
                                        /**< if no timer for this id is registered \a 0 is returned. */

        unsigned numEvents() const;     ///< Returns the number of pending timer events

        void clear();                   ///< Clears all pending timer events

        StatisticsData callbackDurationStats(); ///< Returns and clears the timeInCallback statistics

    private:
#ifndef DOXYGEN
        struct Entry {
            ClockService::clock_type timeout;
            IdType id;
            Callback cb;

            Entry(ClockService::clock_type const & _timeout, IdType _id, Callback _cb)
                : timeout(_timeout), id(_id), cb(_cb) { }
        };
        struct ChangeTimeout {
            senf::ClockService::clock_type timeout_;
            ChangeTimeout(senf::ClockService::clock_type const & t);
            void operator()(Entry & entry);
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

        EntrySet_t entrySet_;
        EntrySetById_t & entrySetById_;
        EntrySetByTimeout_t & entrySetByTimeout_;

        scheduler::TimerEvent timer_;
        ClockService::clock_type callbackDurationThreshold_;
        StatisticAccumulator<std::uint64_t> callbackDurations_;
        DurationExceededCallback durationExceededCallback_;
        
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
