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
    \brief FdDispatcher public header */

#ifndef HH_FdDispatcher_
#define HH_FdDispatcher_ 1

// Custom includes
#include <map>
#include "FdManager.hh"
#include "FIFORunner.hh"

//#include "FdDispatcher.mpp"
#include "FdDispatcher.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace scheduler {

    /** \brief Scheduler dispatcher managing poll-able file descriptors

        File descriptors are added directly to the event loop.
      */
    class FdDispatcher
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::function<void (int)> Callback;

        enum Events { 
            EV_READ = FdManager::EV_READ, EV_PRIO = FdManager::EV_PRIO, EV_WRITE = FdManager::EV_WRITE,
            EV_HUP = FdManager::EV_HUP, EV_ERR = FdManager::EV_ERR,
            EV_ALL = FdManager::EV_READ | FdManager::EV_WRITE | FdManager::EV_PRIO
        };

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{
        
        FdDispatcher(FdManager & manager, FIFORunner & runner);
        ~FdDispatcher();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        bool add(std::string const & name, int fd, Callback const & cb, int events = EV_ALL);
                                        ///< Add file descriptor callback
                                        /**< There is always one active callback for each
                                             combination of file descriptor and event. Registering a
                                             new callback will overwrite the old callback.
                                             \param[in] name descriptive name
                                             \param[in] fd file descriptor
                                             \param[in] cb callback
                                             \param[in] events Events to call \a cb for */

        void remove(int fd, int events = EV_ALL); ///< Remove callback
                                        /**< \param[in] fd file descriptor
                                             \param[in] events Events for which to remove the
                                                 callback */
        
        bool empty() const;             ///< \c true, if no file descriptors are registered.

    protected:

    private:
        /// Internal: File descriptor event
        struct FdEvent 
            : public detail::FdTask<0, FdEvent>,
              public detail::FdTask<1, FdEvent>,
              public detail::FdTask<2, FdEvent>,
              public FdManager::Event
        {
            typedef detail::FdTask<0, FdEvent> ReadTask;
            typedef detail::FdTask<1, FdEvent> PrioTask;
            typedef detail::FdTask<2, FdEvent> WriteTask;

            explicit FdEvent(std::string const & name)
                : ReadTask (name), PrioTask (name), WriteTask (name) {}

            virtual void signal(int events);
            int activeEvents() const;
            int events;
        };

        typedef std::map<int, FdEvent> FdMap;

        FdMap fds_;
        FdManager & manager_;
        FIFORunner & runner_;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "FdDispatcher.cci"
#include "FdDispatcher.ct"
#include "FdDispatcher.cti"
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
