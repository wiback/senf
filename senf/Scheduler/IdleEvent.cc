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
    \brief IdleEvent non-inline non-template implementation */

#include "IdleEvent.hh"
#include "IdleEvent.ih"

// Custom includes

//#include "IdleEvent.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::scheduler::IdleEvent

prefix_ void senf::scheduler::IdleEvent::v_run()
{
    cb_();
}

prefix_ char const * senf::scheduler::IdleEvent::v_type()
    const
{
    return "id";
}

prefix_ std::string senf::scheduler::IdleEvent::v_info()
    const
{
    return "";
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::scheduler::detail::IdleEventDispatcher

prefix_ void senf::scheduler::detail::IdleEventDispatcher::remove(IdleEvent & event)
{
    EventList::iterator i (EventList::s_iterator_to(event));
    if (i == events_.end())
        return;
    FIFORunner::instance().dequeue(&event);
    events_.erase(i);
    if (events_.empty())
        detail::FileDispatcher::instance().timeout(managerTimeout_);
}

prefix_ void senf::scheduler::detail::IdleEventDispatcher::prepareRun()
{
    for (EventList::iterator i (events_.begin()), i_end (events_.end()); i != i_end; ++i)
        i->setRunnable();
}

prefix_ senf::scheduler::detail::IdleEventDispatcher::~IdleEventDispatcher()
{
    for (EventList::iterator i (events_.begin()), i_end (events_.end()); i != i_end; ++i)
        FIFORunner::instance().dequeue(&(*i));
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "IdleEvent.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
