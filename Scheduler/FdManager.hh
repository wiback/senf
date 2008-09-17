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

#ifndef HH_FdManager_
#define HH_FdManager_ 1

// Custom includes
#include "Poller.hh"
#include "ClockService.hh"
#include "../Utils/singleton.hh"

//#include "FdManager.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    class Scheduler;

namespace scheduler {

    /** \brief Manage file descriptor event processing

        The FdManager is the internal class which manages all events (all events need to somehow be
        made accessible via a file descriptor). File descriptors are added or removed from the
        FdManager which then allows waiting until an event occurs on one of the descriptors.

        Registered events must be derived from FdManager::Event. The FdManager does \e not manage
        the event classes, it just manages pointers to externally owned events (the events are owned
        by the respective dispatchers). 

        When an event is posted, it's \c signal() member is called. However, this call will \e not
        execute the user callback registered for the event, it will just mark the relevant tasks as
        runnable.

        \implementation
      */
    class FdManager
        : public singleton<FdManager>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        ///< Event baseclass
        struct Event {
            virtual ~Event();
            virtual void signal(int events) = 0; ///< Called when the given event is posted
        };

        enum Events { 
            EV_READ = Poller<Event>::EV_READ, EV_PRIO = Poller<Event>::EV_PRIO, EV_WRITE = Poller<Event>::EV_WRITE,
            EV_HUP = Poller<Event>::EV_HUP, EV_ERR = Poller<Event>::EV_ERR 
        };

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        using singleton<FdManager>::instance;
        using singleton<FdManager>::alive;

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        bool set(int fd, int events, Event * entry); ///< Set file descriptor event mask
                                        /**< This sets the event mask for \a fd to \a events which
                                             is a combination of values from the \c Events enum. If
                                             \a fd is already registered, the registration is
                                             changed to conform to the parameters passed, otherwise
                                             a new registration is added.
                                             \param[in] fd file descriptor
                                             \param[in] events events to register for
                                             \param[in] entry event to signal 
                                             \returns \c true, if \a fd supports polling, \c false
                                                 otherwise */
        void remove(int fd);            ///< Remove \a fd from the manager

        void timeout(int t);            ///< Set event timeout
                                        /**< proceseOnce() will wait for max \a t milliseconds for
                                             an event to occur. If set to -1, processOnce() will
                                             wait forever. */
        int timeout() const;            ///< Get  timeout in milliseconds

        void processOnce();             ///< Wait for events
                                        /**< This call waits until at least one event is posted but
                                             no longer than the current timeout(). */

        ClockService::clock_type eventTime() const; ///< Time of last event

    protected:

    private:
        FdManager();

        Poller<Event> poller_;
        senf::ClockService::clock_type eventTime_;

        friend class singleton<FdManager>;
        friend class senf::Scheduler;
    };

}}

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
