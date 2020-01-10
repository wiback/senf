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
    \brief FdManager non-inline non-template implementation */

#include "FdManager.hh"
//#include "FdManager.ih"

// Custom includes

//#include "FdManager.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ void senf::scheduler::detail::FdManager::processOnce()
{
    Poller<Event>::range events (poller_.wait());
    eventTime_ = ClockService::now();
    eventTimestamp_.update(eventTime_);
    for (Poller<Event>::iterator i (events.begin()); i != events.end(); ++i)
        i->second->signal(i->first);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "FdManager.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
