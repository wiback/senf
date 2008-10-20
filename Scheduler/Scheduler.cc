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

#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {
    bool terminate_ (false);
}

prefix_ void senf::scheduler::terminate()
{
    terminate_ = true;
}

prefix_ void senf::scheduler::process()
{
    terminate_ = false;
    while(! terminate_ && ! (detail::FdDispatcher::instance().empty() &&
                             detail::TimerDispatcher::instance().empty() &&
                             detail::FileDispatcher::instance().empty())) {
        detail::SignalDispatcher::instance().unblockSignals();
        detail::TimerDispatcher::instance().unblockSignals();
        detail::FdManager::instance().processOnce();
        detail::TimerDispatcher::instance().blockSignals();
        detail::SignalDispatcher::instance().blockSignals();
        detail::FileDispatcher::instance().prepareRun();
        detail::EventEventDispatcher::instance().prepareRun();
        detail::FIFORunner::instance().run();
    }
}

prefix_ void senf::scheduler::restart()
{
    detail::FdManager*            fdm (&detail::FdManager::instance());
    detail::FIFORunner*           ffr (&detail::FIFORunner::instance());
    detail::FdDispatcher*         fdd (&detail::FdDispatcher::instance());
    detail::TimerDispatcher*      tdd (&detail::TimerDispatcher::instance());
    detail::SignalDispatcher*     sdd (&detail::SignalDispatcher::instance());
    detail::FileDispatcher*       fld (&detail::FileDispatcher::instance());
    detail::EventEventDispatcher* eed (&detail::EventEventDispatcher::instance());

    eed->~EventEventDispatcher();
    fld->~FileDispatcher();
    sdd->~SignalDispatcher();
    tdd->~TimerDispatcher();
    fdd->~FdDispatcher();
    ffr->~FIFORunner();
    fdm->~FdManager();
    
    new (fdm) detail::FdManager();
    new (ffr) detail::FIFORunner();
    new (fdd) detail::FdDispatcher();
    new (tdd) detail::TimerDispatcher();
    new (sdd) detail::SignalDispatcher();
    new (fld) detail::FileDispatcher();
    new (eed) detail::EventEventDispatcher();
}

prefix_ bool senf::scheduler::empty()
{
    return detail::FdDispatcher::instance().empty() 
        && detail::TimerDispatcher::instance().empty()
        && detail::FileDispatcher::instance().empty()
        && detail::SignalDispatcher::instance().empty()
        && detail::EventEventDispatcher::instance().empty();
}

///////////////////////////////////////////////////////////////////////////
// senf::schedulerLogTimeSource

prefix_ senf::log::time_type senf::scheduler::LogTimeSource::operator()()
    const
{
    return eventTime();
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
