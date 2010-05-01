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
    \brief FdManager public header */

#ifndef HH_SENF_Scheduler_FdManager_
#define HH_SENF_Scheduler_FdManager_ 1

// Custom includes
#include "Poller.hh"
#include "ClockService.hh"
#include <senf/Utils/singleton.hh>

//#include "FdManager.mpp"
///////////////////////////////hh.p////////////////////////////////////////

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

///////////////////////////////hh.e////////////////////////////////////////
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
