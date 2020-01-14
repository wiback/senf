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
    \brief FdManager public header */

#ifndef HH_SENF_Scheduler_FdManager_
#define HH_SENF_Scheduler_FdManager_ 1

// Custom includes
#include "Poller.hh"
#include "ClockService.hh"
#include <senf/Utils/singleton.hh>
#include <senf/Utils/FlowStatistics.hh>

//#include "FdManager.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace scheduler {

    void restart();

namespace detail {

    class FdManager
        : public singleton<FdManager>
    {
    public:
        ///< Event baseclass
        struct Event {
            virtual ~Event();
            virtual void signal(int events) = 0; ///< Called when the given event is posted
        };

        enum Events {
            EV_READ = Poller<Event>::EV_READ, EV_PRIO = Poller<Event>::EV_PRIO, EV_WRITE = Poller<Event>::EV_WRITE,
            EV_HUP = Poller<Event>::EV_HUP, EV_ERR = Poller<Event>::EV_ERR
        };

        using singleton<FdManager>::instance;
        using singleton<FdManager>::alive;

        bool set(int fd, int events, Event * entry);
        void remove(int fd);

        // Called by FileDispatcher
        void timeout(int t);
        int timeout() const;

        void processOnce();             ///< Wait for events
                                        /**< This call waits until at least one event is posted but
                                             no longer than the current timeout(). */
        ClockService::clock_type const & eventTime() const; ///< Time of last event
        CyclicTimestamp const & eventTimestamp() const; ///< Cyclic Timestamp of last event (1ms precision)
        
    protected:

    private:
        FdManager();

        Poller<Event> poller_;
        ClockService::clock_type eventTime_;
        CyclicTimestamp eventTimestamp_;

        friend void senf::scheduler::restart();
        friend class singleton<FdManager>;
    };

}}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "FdManager.cci"
//#include "FdManager.ct"
//#include "FdManager.cti"
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
