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

//#include "FdManager.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace scheduler {

    /** \brief
      */
    class FdManager
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        struct Event {
            virtual ~Event();
            virtual void signal(int events) = 0;
        };

        enum Events { 
            EV_READ = Poller<Event>::EV_READ, EV_PRIO = Poller<Event>::EV_PRIO, EV_WRITE = Poller<Event>::EV_WRITE,
            EV_HUP = Poller<Event>::EV_HUP, EV_ERR = Poller<Event>::EV_ERR 
        };

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        void set(int fd, int events, Event * entry);
        void remove(int fd);

        void timeout(int t);
        int timeout() const;

        void processOnce();

    protected:

    private:
        Poller<Event> poller_;
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
