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
    \brief TimerDispatcher public header */

#ifndef HH_TimerDispatcher_
#define HH_TimerDispatcher_ 1

// Custom includes
#include <signal.h>
#include <set>
#include <map>
#include "ClockService.hh"
#include "FdManager.hh"
#include "FIFORunner.hh"

//#include "TimerDispatcher.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace scheduler {

    /** \brief
      */
    class TimerDispatcher
        : public FdManager::Event
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::function<void ()> Callback;
        typedef unsigned timer_id;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        TimerDispatcher(FdManager & manager, FIFORunner & runner);
        ~TimerDispatcher();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        timer_id add(ClockService::clock_type timeout, Callback const & cb);
        void remove(timer_id id);

        void blockSignals();
        void unblockSignals();

    protected:

    private:
        struct TimerEvent
            : public FIFORunner::TaskInfo
        {
            TimerEvent(timer_id id_, Callback const & cb_, TimerDispatcher & dispatcher_);
            virtual void run();

            timer_id id;
            Callback cb;
            TimerDispatcher & dispatcher;
        };

        virtual void signal(int events);
        static void sigHandler(int signal, ::siginfo_t * siginfo, void *);
        void reschedule();

        FdManager & manager_;
        FIFORunner & runner_;

        typedef std::multimap<ClockService::clock_type, TimerEvent> TimerMap;
        typedef std::map<int, TimerMap::iterator> TimerIdMap;

        TimerMap timers_;
        TimerIdMap timerIdIndex_;
        timer_id lastId_;

        int timerPipe_[2];
        sigset_t sigSet_;
        bool blocked_;
        timer_t timerId_;

        static unsigned useCount_;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "TimerDispatcher.cci"
//#include "TimerDispatcher.ct"
//#include "TimerDispatcher.cti"
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
