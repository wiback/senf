// $Id$
//
// Copyright (C) 2006
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
    \brief Scheduler non-inline non-template implementation

    \idea Multithreading support: To support multithreading, the
    static member Scheduler::instance() must return a thread-local
    value (that is Scheduler::instance() must allocate one Scheduler
    instance per thread). Another possibility would be to distribute
    the async load unto several threads (one scheduler for multiple
    threads)
 */

#include "Scheduler.hh"
//#include "Scheduler.ih"

// Custom includes
#include "SignalEvent.hh"

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

prefix_ void senf::Scheduler::process()
{
    terminate_ = false;
    while(! terminate_ && ! (fdDispatcher_.empty() &&
                             timerDispatcher_.empty() &&
                             fileDispatcher_.empty())) {
        scheduler::detail::SignalDispatcher::instance().unblockSignals();
        timerDispatcher_.unblockSignals();
        scheduler::FdManager::instance().processOnce();
        timerDispatcher_.blockSignals();
        scheduler::detail::SignalDispatcher::instance().blockSignals();
        fileDispatcher_.prepareRun();
        scheduler::FIFORunner::instance().run();
    }
}

prefix_ void senf::Scheduler::restart()
{
    scheduler::FdManager* fdm (&scheduler::FdManager::instance());
    scheduler::FIFORunner* ffr (&scheduler::FIFORunner::instance());
    scheduler::FdDispatcher* fdd (&fdDispatcher_);
    scheduler::TimerDispatcher* td (&timerDispatcher_);
    scheduler::detail::SignalDispatcher* sd (&scheduler::detail::SignalDispatcher::instance());
    scheduler::FileDispatcher* fld (&fileDispatcher_);
    
    fld->~FileDispatcher();
    sd->~SignalDispatcher();
    td->~TimerDispatcher();
    fdd->~FdDispatcher();
    ffr->~FIFORunner();
    fdm->~FdManager();
    
    new (fdm) scheduler::FdManager();
    new (ffr) scheduler::FIFORunner();
    new (fdd) scheduler::FdDispatcher(*fdm, *ffr);
    new (td) scheduler::TimerDispatcher(*fdm, *ffr);
    new (sd) scheduler::detail::SignalDispatcher();
    new (fld) scheduler::FileDispatcher(*fdm, *ffr);
}

///////////////////////////////////////////////////////////////////////////
// senf::SchedulerLogTimeSource

prefix_ senf::log::time_type senf::SchedulerLogTimeSource::operator()()
    const
{
    return Scheduler::instance().eventTime();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
