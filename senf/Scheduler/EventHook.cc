//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief EventHook non-inline non-template implementation */

#include "EventHook.hh"
#include "EventHook.ih"

// Custom includes

//#include "EventHook.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::scheduler::EventHook

prefix_ void senf::scheduler::EventHook::v_run()
{
    cb_();
}

prefix_ char const * senf::scheduler::EventHook::v_type()
    const
{
    return "ee";
}

prefix_ std::string senf::scheduler::EventHook::v_info()
    const
{
    return "";
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::scheduler::detail::EventHookDispatcher

prefix_ senf::scheduler::detail::EventHookDispatcher::~EventHookDispatcher()
{
    for (EventList::iterator i (events_.begin()); i != events_.end(); ++i)
        FIFORunner::instance().dequeue(&(*i));
}

prefix_ prefix_ void senf::scheduler::detail::EventHookDispatcher::remove(EventHook & event)
{
    EventList::iterator i (EventList::s_iterator_to(event));
    if (i == events_.end())
        return;
    FIFORunner::instance().dequeue(&event);
    events_.erase(i);
}

prefix_ void senf::scheduler::detail::EventHookDispatcher::prepareRun()
{
    for (EventList::iterator i (events_.begin()); i != events_.end(); ++i)
        i->setRunnable();
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "EventHook.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
