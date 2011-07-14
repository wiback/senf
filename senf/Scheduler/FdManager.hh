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
    \brief FdManager public header */

#ifndef HH_SENF_Scheduler_FdManager_
#define HH_SENF_Scheduler_FdManager_ 1

// Custom includes
#include "Poller.hh"
#include "ClockService.hh"
#include <senf/Utils/singleton.hh>

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
        ClockService::clock_type eventTime() const; ///< Time of last event

    protected:

    private:
        FdManager();

        Poller<Event> poller_;
        senf::ClockService::clock_type eventTime_;

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
